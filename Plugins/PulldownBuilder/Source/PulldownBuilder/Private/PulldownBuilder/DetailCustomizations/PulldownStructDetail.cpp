// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/DetailCustomizations/PulldownStructDetail.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownBuilder/Utilities/PulldownBuilderAppearanceSettings.h"
#include "PulldownBuilder/Widgets/SPulldownSelectorComboButton.h"
#include "PulldownBuilder/Types/PulldownStructType.h"
#include "PulldownBuilder/Types/PulldownRow.h"
#include "PulldownBuilder/Types/StructContainer.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "DetailWidgetRow.h"
#include "PropertyEditorModule.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailPropertyRow.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Modules/ModuleManager.h"
#if UE_5_01_OR_LATER
#include "Styling/AppStyle.h"
#else
#include "EditorStyleSet.h"
#endif
#include "Editor.h"

#define LOCTEXT_NAMESPACE "PulldownStructDetail"

namespace PulldownBuilder
{
	namespace PulldownStructDetailDefine
	{
		static const FName MovieSceneSignedObjectClassName = TEXT("MovieSceneSignedObject");
	}
	
	void FPulldownStructDetail::Register(const FPulldownStructType& StructType)
	{
		if (!StructType.IsValid())
		{
			UE_LOG(LogPulldownBuilder, Warning, TEXT("You have tried to register an empty struct..."));
			return;
		}
	
		auto& PropertyEditorModule = FPulldownBuilderUtils::GetPropertyEditorModule();
		PropertyEditorModule.RegisterCustomPropertyTypeLayout(
			*StructType,
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FPulldownStructDetail::MakeInstance)
		);
	}

	void FPulldownStructDetail::Unregister(const FPulldownStructType& StructType)
	{
		if (!StructType.IsValid())
		{
			UE_LOG(LogPulldownBuilder, Warning, TEXT("You have tried to unregister an empty struct..."));
			return;
		}

		auto& PropertyEditorModule = FPulldownBuilderUtils::GetPropertyEditorModule();
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout(
			*StructType
		);
	}

	TSharedRef<IPropertyTypeCustomization> FPulldownStructDetail::MakeInstance()
	{
		return MakeShared<FPulldownStructDetail>();
	}

	void FPulldownStructDetail::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
	{
		StructPropertyHandle = InStructPropertyHandle;
		check(StructPropertyHandle.IsValid());

		// Sets the handles for properties that need to be customized.
		CustomizationProperties.Add(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue), &SelectedValueHandle);
		CustomizationProperties.Add(FPulldownStructBase::SearchableObjectPropertyName, &SearchableObjectHandle);
		CustomizationProperties.Add(FPulldownStructBase::IsEditedPropertyName, &IsEditedHandle);
		
		// Initializes handles for properties that need to be customized.
		CustomizationProperties.Initialize(StructPropertyHandle.ToSharedRef());
		check(CustomizationProperties.IsInitialized());
	
		HeaderRow
			.NameContent()
			[
				StructPropertyHandle->CreatePropertyNameWidget()
			];

		// If the property is only FPulldownStructBase::SelectedValue, displays it inline.
		uint32 NumChildProperties;
		StructPropertyHandle->GetNumChildren(NumChildProperties);
		if (NumChildProperties == CustomizationProperties.Num() && UPulldownBuilderAppearanceSettings::Get().bShouldInlineDisplayWhenSingleProperty)
		{
			ConstructDetailWidgetRow(HeaderRow);
			
			InitializePulldown();
		}
	}

	void FPulldownStructDetail::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
	{
		check(StructPropertyHandle.IsValid() && SelectedValueHandle.IsValid());

		// Adds child properties other than properties that need to be customized to the StructBuilder.
		uint32 NumChildProperties;
		StructPropertyHandle->GetNumChildren(NumChildProperties);
		for (uint32 Index = 0; Index < NumChildProperties; Index++)
		{
			const TSharedPtr<IPropertyHandle> ChildPropertyHandle = StructPropertyHandle->GetChildHandle(Index);
			if (ChildPropertyHandle.IsValid())
			{
#if UE_4_25_OR_LATER
				if (FProperty* ChildProperty = ChildPropertyHandle->GetProperty())
#else
				if (UProperty* ChildProperty = ChildPropertyHandle->GetProperty())
#endif
				{
					if (!CustomizationProperties.Contains(ChildProperty))
					{
						StructBuilder.AddProperty(ChildPropertyHandle.ToSharedRef());
						ChildPropertyHandle->SetOnPropertyValueChanged(
							FSimpleDelegate::CreateSP(this, &FPulldownStructDetail::RebuildPulldown)
						);
					}
				}
			}
		}

		// If there are multiple properties, do not display inline.
		if (NumChildProperties > CustomizationProperties.Num() || !UPulldownBuilderAppearanceSettings::Get().bShouldInlineDisplayWhenSingleProperty)
		{
			AddCustomRowBeforeSelectedValue(StructBuilder);

			IDetailPropertyRow& PropertyRow = StructBuilder.AddProperty(SelectedValueHandle.ToSharedRef());
#if !UE_5_01_OR_LATER
			PropertyRow.OverrideResetToDefault(
				FResetToDefaultOverride::Create(
					FIsResetToDefaultVisible::CreateRaw(this, &FPulldownStructDetail::CanResetToDefaultAction),
					FResetToDefaultHandler::CreateRaw(this, &FPulldownStructDetail::OnResetToDefaultAction)
				)
			);
#endif
			FDetailWidgetRow& WidgetRow = PropertyRow.CustomWidget();
			WidgetRow.NameContent()
			[
				SelectedValueHandle->CreatePropertyNameWidget()
			];
			
			ConstructDetailWidgetRow(WidgetRow);
			
			AddCustomRowAfterSelectedValue(StructBuilder);

			InitializePulldown();
		}
	}

	void FPulldownStructDetail::InitializePulldown()
	{
		check(IsEditedHandle.IsValid());
		
		SelectableValues = GenerateSelectableValues();

		// If a default value is set and the selected value is either None or other than the default value, marks it as edited.
		bool bNeedToMarkEdited = true;
		const TSharedPtr<FPulldownRow> DefaultRow = SelectableValues.GetDefaultRow();
		if (DefaultRow.IsValid())
		{
			FName CurrentSelectedValue;
			if (SelectedValueHandle->GetValue(CurrentSelectedValue) == FPropertyAccess::Success)
			{
				if ((CurrentSelectedValue == NAME_None) || (CurrentSelectedValue == *DefaultRow->SelectedValue))
				{
					bNeedToMarkEdited = false;
				}
			}
		}
		if (bNeedToMarkEdited)
		{
			SetPropertyValueSafe(IsEditedHandle.ToSharedRef(), true);
		}
		
		UpdateSearchableObject();
		ApplyDefaultValue();
		RefreshPulldownWidget();
	}

	void FPulldownStructDetail::RebuildPulldown()
	{
		check(StructPropertyHandle.IsValid());

		// Finds PulldownContents in the property struct and build a list of strings to display in the pull-down menu.
		void* StructValueData = nullptr;
		if (StructPropertyHandle->GetValueData(StructValueData) == FPropertyAccess::Success)
		{
			SelectableValues = GenerateSelectableValues();
		}
		// Empties the list if data acquisition fails or if multiple selections are made.
		else
		{
			OnMultipleSelected();
			return;
		}

		UpdateSearchableObject();
		ApplyDefaultValue();
		RefreshPulldownWidget();
	}

	void FPulldownStructDetail::RefreshPulldownWidget()
	{
		check(SelectedValueHandle.IsValid());
	
		// Checks if the currently set string is included in the constructed list.
		FName CurrentSelectedValue;
		SelectedValueHandle->GetValue(CurrentSelectedValue);

		TSharedPtr<FPulldownRow> SelectedItem = FindSelectableValueByName(CurrentSelectedValue);
		if (!SelectedItem.IsValid())
		{
			SetPropertyValueSafe(SelectedValueHandle.ToSharedRef(), NAME_None);
			SelectedItem = FindSelectableValueByName(NAME_None);
		}

		if (SelectedValueWidget.IsValid())
		{
			SelectedValueWidget->RefreshList();
			SelectedValueWidget->SetSelectedItem(SelectedItem);
		}
	}

	FPulldownRows FPulldownStructDetail::GenerateSelectableValues()
	{
		check(StructPropertyHandle.IsValid());
	
#if UE_4_25_OR_LATER
		if (const auto* StructProperty = CastField<FStructProperty>(StructPropertyHandle->GetProperty()))
#else
		if (const auto* StructProperty = Cast<UStructProperty>(StructPropertyHandle->GetProperty()))
#endif
		{
			TArray<UObject*> OuterObjects;
			StructPropertyHandle->GetOuterObjects(OuterObjects);

			void* RawData;
			const FPropertyAccess::Result Result = StructPropertyHandle->GetValueData(RawData);
			if (Result != FPropertyAccess::Success)
			{
				RawData = nullptr;
			}
			
			return FPulldownBuilderUtils::GetPulldownRowsFromStruct(
				StructProperty->Struct,
				OuterObjects,
				FStructContainer(StructProperty->Struct, static_cast<uint8*>(RawData))
			);
		}

		return FPulldownRows::Empty;
	}

	void FPulldownStructDetail::OnMultipleSelected()
	{
		SelectableValues.Reset();
	}

	UPulldownContents* FPulldownStructDetail::GetRelatedPulldownContents() const
	{
		if (const UScriptStruct* RelatedPulldownStructType = GetRelatedPulldownStructType())
		{
			return FPulldownBuilderUtils::FindPulldownContentsByStruct(RelatedPulldownStructType);
		}

		return nullptr;
	}

	const UScriptStruct* FPulldownStructDetail::GetRelatedPulldownStructType() const
	{
		check(StructPropertyHandle.IsValid());
		
#if UE_4_25_OR_LATER
		if (const auto* StructProperty = CastField<FStructProperty>(StructPropertyHandle->GetProperty()))
#else
		if (const auto* StructProperty = Cast<UStructProperty>(StructPropertyHandle->GetProperty()))
#endif
		{
			return StructProperty->Struct;
		}

		return nullptr;
	}

	TSharedRef<SWidget> FPulldownStructDetail::GenerateSelectableValuesWidget()
	{
		return
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			[
				SAssignNew(SelectedValueWidget, SPulldownSelectorComboButton)
				.ListItemsSource(&SelectableValues)
				.GetSelection(this, &FPulldownStructDetail::GetSelection)
				.HeightOverride(this, &FPulldownStructDetail::GetIndividualPanelHeight)
				.WidthOverride(this, &FPulldownStructDetail::GetIndividualPanelWidth)
				.OnSelectionChanged(this, &FPulldownStructDetail::OnSelectedValueChanged)
				.OnComboBoxOpened(this, &FPulldownStructDetail::RebuildPulldown)
			];
	}

	void FPulldownStructDetail::SetPropertyValueSafeImpl(
		const TSharedRef<IPropertyHandle>& TargetPropertyHandle,
		const TFunction<void(const TSharedRef<IPropertyHandle>& PropertyHandle)>& Predicate
	)
	{
		auto IsMovieSceneClass = [](const UClass* TestClass) -> bool
		{
			const UClass* Class = TestClass;
			while (IsValid(Class))
			{
				if (Class->GetFName() == PulldownStructDetailDefine::MovieSceneSignedObjectClassName)
				{
					return true;
				}

				Class = Class->GetSuperClass();
			}

			return false;
		};
			
		auto GetOuterBaseClass = [](const TSharedRef<IPropertyHandle>& PropertyHandle) -> const UClass*
		{
#if UE_5_00_OR_LATER
			return PropertyHandle->GetOuterBaseClass();
#else
			TArray<UObject*> OuterObjects;
			PropertyHandle->GetOuterObjects(OuterObjects);
			if (OuterObjects.IsValidIndex(0))
			{
				return OuterObjects[0]->GetClass();
			}

			return nullptr;
#endif
		};
			
		const UClass* OuterBaseClass = GetOuterBaseClass(TargetPropertyHandle);
		if (IsMovieSceneClass(OuterBaseClass))
		{
			check(IsValid(GEditor));
			TWeakPtr<IPropertyHandle> WeakPropertyHandle = TargetPropertyHandle;
			GEditor->GetTimerManager()->SetTimerForNextTick(
				[WeakPropertyHandle, Predicate]()
				{
					if (WeakPropertyHandle.IsValid())
					{
						Predicate(WeakPropertyHandle.Pin().ToSharedRef());
					}
				}
			);
		}
		else
		{
			Predicate(TargetPropertyHandle);
		}
	}

	TSharedPtr<FPulldownRow> FPulldownStructDetail::FindSelectableValueByName(const FName& InName) const
	{
		return SelectableValues.FindByPredicate(
			[&](const TSharedPtr<FPulldownRow>& Row)
			{
				return (Row.IsValid() && Row->SelectedValue.Equals(InName.ToString()));
			}
		);
	}

	TSharedPtr<FPulldownRow> FPulldownStructDetail::GetSelection() const
	{
		FName SelectedValue = NAME_None;
		if (SelectedValueHandle.IsValid())
		{
			SelectedValueHandle->GetValue(SelectedValue);
		}

		return FindSelectableValueByName(SelectedValue);
	}

	float FPulldownStructDetail::GetIndividualPanelHeight() const
	{
		if (const UPulldownContents* PulldownContents = GetRelatedPulldownContents())
		{
			const TOptional<FVector2D>& IndividualPanelSize = PulldownContents->GetIndividualPanelSize();
			if (IndividualPanelSize.IsSet())
			{
				return IndividualPanelSize.GetValue().Y;
			}
		}

		return 0.f;
	}

	float FPulldownStructDetail::GetIndividualPanelWidth() const
	{
		if (const UPulldownContents* PulldownContents = GetRelatedPulldownContents())
		{
			const TOptional<FVector2D>& IndividualPanelSize = PulldownContents->GetIndividualPanelSize();
			if (IndividualPanelSize.IsSet())
			{
				return IndividualPanelSize.GetValue().X;
			}
		}

		return 0.f;
	}

	void FPulldownStructDetail::OnSelectedValueChanged(TSharedPtr<FPulldownRow> SelectedItem, ESelectInfo::Type SelectInfo)
	{
		check(SelectedValueHandle.IsValid() && IsEditedHandle.IsValid());
		
		if (!SelectedItem.IsValid())
		{
			return;
		}
		
		const FName NewSelectedValue = *SelectedItem->SelectedValue;
		FName OldSelectedValue;
		SelectedValueHandle->GetValue(OldSelectedValue);
		if (NewSelectedValue != OldSelectedValue)
		{
			SetPropertyValueSafe(SelectedValueHandle.ToSharedRef(), NewSelectedValue);
			SetPropertyValueSafe(IsEditedHandle.ToSharedRef(), true);
		}
	}

	void FPulldownStructDetail::UpdateSearchableObject()
	{
		check(SearchableObjectHandle.IsValid());

		const UScriptStruct* RelatedPulldownStructType = GetRelatedPulldownStructType();
		if (!IsValid(RelatedPulldownStructType))
		{
			return;
		}

		if (!FPulldownBuilderUtils::HasPulldownStructPostSerialize(RelatedPulldownStructType))
		{
			return;
		}
		
		UObject* SearchableObject;
		if (SearchableObjectHandle->GetValue(SearchableObject) != FPropertyAccess::Success)
		{
			SearchableObject = nullptr;
		}

		const UObject* NewSearchableObject = nullptr;
		if (const UPulldownContents* RelatedPulldownContents = GetRelatedPulldownContents())
		{
			if (!IsValid(SearchableObject) || (SearchableObject != RelatedPulldownContents))
			{
				NewSearchableObject = RelatedPulldownContents;
			}
		}
		if (IsValid(NewSearchableObject))
		{
			FWeakObjectPtr WeakNewSearchableObject = NewSearchableObject;
			SetPropertyValueSafeImpl(
				SearchableObjectHandle.ToSharedRef(),
				[WeakNewSearchableObject](const TSharedRef<IPropertyHandle>& PropertyHandle)
				{
					if (WeakNewSearchableObject.IsValid())
					{
						PropertyHandle->SetValue(WeakNewSearchableObject.Get());
					}
				}
			);
		}
	}

	void FPulldownStructDetail::ApplyDefaultValue(const bool bForceApply /* = false */)
	{
		if (!bForceApply && IsEdited())
		{
			return;
		}
		
		const TSharedPtr<FPulldownRow> DefaultRow = SelectableValues.GetDefaultRow();
		const FName DefaultValue = (DefaultRow.IsValid() ? FName(*DefaultRow->SelectedValue) : FName(NAME_None));
		SetPropertyValueSafe(SelectedValueHandle.ToSharedRef(), DefaultValue);
		SetPropertyValueSafe(IsEditedHandle.ToSharedRef(), false);
	}

	bool FPulldownStructDetail::IsEdited() const
	{
		check(IsEditedHandle.IsValid());
		
		bool bValue;
		if (IsEditedHandle->GetValue(bValue) == FPropertyAccess::Success)
		{
			return bValue;
		}

		return false;
	}

	void FPulldownStructDetail::ConstructDetailWidgetRow(FDetailWidgetRow& DetailWidgetRow)
	{
		DetailWidgetRow
			.PropertyHandleList(TArray<TSharedPtr<IPropertyHandle>>{ SelectedValueHandle })
			.CopyAction(CreateSelectedValueCopyAction())
			.PasteAction(CreateSelectedValuePasteAction())
			.ValueContent()
			.MinDesiredWidth(500)
			[
				GenerateSelectableValuesWidget()
			]
			.AddCustomContextMenuAction(
				FUIAction
				(
					FExecuteAction::CreateSP(this, &FPulldownStructDetail::OnBrowseSourceAssetAction),
					FCanExecuteAction::CreateSP(this, &FPulldownStructDetail::CanBrowseSourceAssetAction)
				),
				LOCTEXT("OpenSourceAssetLabel", "Open Source Asset"),
				LOCTEXT("OpenSourceAssetTooltip", "Open the underlying pulldown contents asset for the pin's pulldown struct."),
				FSlateIcon(
#if UE_5_01_OR_LATER
					FAppStyle::GetAppStyleSetName(),
#else
					FEditorStyle::GetStyleSetName(),
#endif
					TEXT("SystemWideCommands.FindInContentBrowser")
				)
			);
		
#if UE_5_01_OR_LATER
		DetailWidgetRow.OverrideResetToDefault(
			FResetToDefaultOverride::Create(
				FIsResetToDefaultVisible::CreateRaw(this, &FPulldownStructDetail::CanResetToDefaultAction),
				FResetToDefaultHandler::CreateRaw(this, &FPulldownStructDetail::OnResetToDefaultAction)
			)
		);
#endif
	}

	FUIAction FPulldownStructDetail::CreateSelectedValueCopyAction()
	{
		return FUIAction
		(
			FExecuteAction::CreateSP(this, &FPulldownStructDetail::OnSelectedValueCopyAction),
			FCanExecuteAction::CreateSP(this, &FPulldownStructDetail::CanSelectedValueCopyAction)
		);
	}

	FUIAction FPulldownStructDetail::CreateSelectedValuePasteAction()
	{
		return FUIAction
		(
			FExecuteAction::CreateSP(this, &FPulldownStructDetail::OnSelectedValuePasteAction),
			FCanExecuteAction::CreateSP(this, &FPulldownStructDetail::CanSelectedValuePasteAction)
		);
	}

	void FPulldownStructDetail::OnSelectedValueCopyAction()
	{
		if (!SelectedValueHandle.IsValid())
		{
			return;
		}

		FName SelectedValue;
		SelectedValueHandle->GetValue(SelectedValue);

		FPlatformApplicationMisc::ClipboardCopy(*SelectedValue.ToString());
	}

	void FPulldownStructDetail::OnSelectedValuePasteAction()
	{
		if (!SelectedValueWidget.IsValid())
		{
			return;
		}
	
		FName PastedText;
		{
			FString ClipboardString;
			FPlatformApplicationMisc::ClipboardPaste(ClipboardString);
			PastedText = *ClipboardString;
		}

		const TSharedPtr<FPulldownRow> SelectedItem = FindSelectableValueByName(PastedText);
		if (SelectedItem.IsValid())
		{
			SelectedValueWidget->SetSelectedItem(SelectedItem);
		}

		RefreshPulldownWidget();
	}
	
	void FPulldownStructDetail::OnResetToDefaultAction(TSharedPtr<IPropertyHandle> PropertyHandle)
	{
		ApplyDefaultValue(true);
		RefreshPulldownWidget();
	}
	
	void FPulldownStructDetail::OnBrowseSourceAssetAction()
	{
		if (UPulldownContents* PulldownContents = GetRelatedPulldownContents())
		{
			FPulldownBuilderUtils::OpenPulldownContents(PulldownContents);
		}
	}

	bool FPulldownStructDetail::CanSelectedValueCopyAction() const
	{
		return true;
	}

	bool FPulldownStructDetail::CanSelectedValuePasteAction() const
	{
		check(StructPropertyHandle.IsValid());
	
		return StructPropertyHandle->IsEditable();
	}

	bool FPulldownStructDetail::CanResetToDefaultAction(TSharedPtr<IPropertyHandle> PropertyHandle) const
	{
		if (SelectableValues.HasDefaultRow())
		{
			return IsEdited();
		}

		const TSharedPtr<FPulldownRow> SelectedRow = GetSelection();
		return (SelectedRow.IsValid() && (FName(SelectedRow->SelectedValue) != NAME_None));
	}

	bool FPulldownStructDetail::CanBrowseSourceAssetAction() const
	{
		return IsValid(GetRelatedPulldownContents());
	}

	void FPulldownStructDetail::FCustomizationProperties::Add(const FName& InPropertyName, TSharedPtr<IPropertyHandle>* InPropertyHandle)
	{
		check(InPropertyHandle != nullptr);

		PropertyNamesToHandles.Emplace(InPropertyName, InPropertyHandle);
	}

	void FPulldownStructDetail::FCustomizationProperties::Initialize(const TSharedRef<IPropertyHandle>& InStructPropertyHandle)
	{
		uint32 NumChildProperties;
		InStructPropertyHandle->GetNumChildren(NumChildProperties);
		for (uint32 Index = 0; Index < NumChildProperties; Index++)
		{
			if (IsInitialized())
			{
				break;
			}
			
			const TSharedPtr<IPropertyHandle> ChildPropertyHandle = InStructPropertyHandle->GetChildHandle(Index);
			if (!ChildPropertyHandle.IsValid())
			{
				continue;
			}

#if UE_4_25_OR_LATER
			const FProperty* ChildProperty = ChildPropertyHandle->GetProperty();
			if (ChildProperty == nullptr)
#else
			const UProperty* ChildProperty = ChildPropertyHandle->GetProperty();
			if (!IsValid(ChildProperty))
#endif
			{
				continue;
			}

			const FName PropertyName = ChildProperty->GetFName();
			if (!PropertyNamesToHandles.Contains(PropertyName))
			{
				continue;
			}

			TSharedPtr<IPropertyHandle>* PropertyHandlePtr = PropertyNamesToHandles.FindRef(PropertyName);
			check(PropertyHandlePtr != nullptr);
			(*PropertyHandlePtr) = ChildPropertyHandle;
		}
	}

	bool FPulldownStructDetail::FCustomizationProperties::IsInitialized() const
	{
		TArray<TSharedPtr<IPropertyHandle>*> PropertyHandles;
		PropertyNamesToHandles.GenerateValueArray(PropertyHandles);

		for (const auto* PropertyHandle : PropertyHandles)
		{
			check(PropertyHandle != nullptr);

			if (!PropertyHandle->IsValid())
			{
				return false;
			}
		}

		return true;
	}

#if UE_4_25_OR_LATER
	bool FPulldownStructDetail::FCustomizationProperties::Contains(FProperty* InProperty) const
#else
	bool FPulldownStructDetail::FCustomizationProperties::Contains(UProperty* InProperty) const
#endif
	{
#if UE_4_25_OR_LATER
		if (InProperty == nullptr)
#else
		if (!IsValid(InProperty))
#endif
		{
			return false;
		}
		
		TArray<FName> PropertyNames;
		PropertyNamesToHandles.GenerateKeyArray(PropertyNames);
		return PropertyNames.Contains(InProperty->GetFName());
	}

	uint32 FPulldownStructDetail::FCustomizationProperties::Num() const
	{
		return PropertyNamesToHandles.Num();
	}
}

#undef LOCTEXT_NAMESPACE
