// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/DetailCustomizations/PulldownStructDetail.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownBuilder/Utilities/PulldownBuilderAppearanceSettings.h"
#include "PulldownBuilder/Utilities/CustomPropertyTypeLayoutRegistry.h"
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
	
	/**
	 * Special implementation is performed because FPulldownStructDetail is inherited and for registering from UScriptStruct.
	 */
	struct FCustomPropertyTypeLayoutRegistries
	{
	private:
		struct FCustomPropertyTypeLayoutRegistry : public ICustomPropertyTypeLayoutRegistry
		{
		public:
			// Constructor.
			explicit FCustomPropertyTypeLayoutRegistry(const UScriptStruct* StructType)
				: ICustomPropertyTypeLayoutRegistry(GetNameSafe(StructType))
			{
			}

		protected:
			// ICustomPropertyTypeLayoutRegistry interface.
			virtual TSharedRef<IPropertyTypeCustomization> MakeInstance() override
			{
				return MakeShared<FPulldownStructDetail>();
			}
			// End of ICustomPropertyTypeLayoutRegistry interface.
		};
	
	public:
#define PREDICATE \
		[&](const TUniquePtr<ICustomPropertyTypeLayoutRegistry>& Registry) -> bool \
		{ \
			check(Registry.IsValid()); \
			return (Registry->GetPropertyTypeName() == GetNameSafe(StructType)); \
		}
		
		static void Register(const UScriptStruct* StructType)
		{
			check(!Registries.ContainsByPredicate(PREDICATE));

			Registries.Add(MakeUnique<FCustomPropertyTypeLayoutRegistry>(StructType));
		}
		
		static void Unregister(const UScriptStruct* StructType)
		{
			check(Registries.ContainsByPredicate(PREDICATE));
			
			const int32 Index = Registries.IndexOfByPredicate(PREDICATE);
			check(Index != INDEX_NONE);

			Registries.RemoveAt(Index);
		}
#undef PREDICATE
		
	private:
		static TArray<TUniquePtr<ICustomPropertyTypeLayoutRegistry>> Registries;
	};
	TArray<TUniquePtr<ICustomPropertyTypeLayoutRegistry>> FCustomPropertyTypeLayoutRegistries::Registries;
	
	void FPulldownStructDetail::Register(const FPulldownStructType& StructType)
	{
		if (!StructType.IsValid())
		{
			UE_LOG(LogPulldownBuilder, Warning, TEXT("You have tried to register an empty struct..."));
			return;
		}

		FCustomPropertyTypeLayoutRegistries::Register(StructType.SelectedStruct);
	}

	void FPulldownStructDetail::Unregister(const FPulldownStructType& StructType)
	{
		if (!StructType.IsValid())
		{
			UE_LOG(LogPulldownBuilder, Warning, TEXT("You have tried to unregister an empty struct..."));
			return;
		}

		FCustomPropertyTypeLayoutRegistries::Unregister(StructType.SelectedStruct);
	}

	void FPulldownStructDetail::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
	{
		StructPropertyHandle = InStructPropertyHandle;
		check(StructPropertyHandle.IsValid());

		// Sets the handles for properties that need to be customized.
		CustomizationProperties.Add(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue), &SelectedValueHandle);
		CustomizationProperties.Add(FPulldownStructBase::CachedDisplayTextPropertyName, &CachedDisplayTextHandle);
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
		
		if (ShouldInlineDisplay())
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
		if (!ShouldInlineDisplay())
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
		
		UpdateSearchableObject();

		// The default value is applied only if the default value is sets and the default value is None.
		// Otherwise, it marks whether the edited or not according to the default value.
		const FName CurrentSelectedValue = [&]() -> FName
		{
			FName SelectedValue;
			if (SelectedValueHandle->GetValue(SelectedValue) != FPropertyAccess::Success)
			{
				SelectedValue = NAME_None;
			}
			return SelectedValue;
		}();
		const TSharedPtr<FPulldownRow> DefaultRow = SelectableValues.GetDefaultRow();
		if (DefaultRow.IsValid())
		{
			if (IsEdited() && (CurrentSelectedValue == *DefaultRow->SelectedValue))
			{
				SetPropertyValueSafe(IsEditedHandle.ToSharedRef(), false);
			}
			else
			{
				if (CurrentSelectedValue == NAME_None)
				{
					ApplyDefaultValue();
				}
				else if (CurrentSelectedValue != *DefaultRow->SelectedValue)
				{
					SetPropertyValueSafe(IsEditedHandle.ToSharedRef(), true);
				}
			}
		}
		else
		{
			if (IsEdited() && (CurrentSelectedValue == NAME_None))
			{
				SetPropertyValueSafe(IsEditedHandle.ToSharedRef(), false);
			}
			else if (!IsEdited() && (CurrentSelectedValue != NAME_None))
			{
				SetPropertyValueSafe(IsEditedHandle.ToSharedRef(), true);
			}
		}
		
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
		RefreshPulldownWidget();
	}

	void FPulldownStructDetail::RefreshPulldownWidget()
	{
		if (!SelectedValueWidget.IsValid())
		{
			return;
		}

		const TSharedPtr<FPulldownRow> SelectedItem = GetSelection();
		SelectedValueWidget->SetSelectedItem(SelectedItem);
		SelectedValueWidget->RefreshList();
	}

	bool FPulldownStructDetail::ShouldInlineDisplay() const
	{
		check(StructPropertyHandle.IsValid());

		if (!UPulldownBuilderAppearanceSettings::Get().bShouldInlineDisplayWhenSingleProperty)
		{
			return false;
		}
		
		uint32 NumChildProperties;
		StructPropertyHandle->GetNumChildren(NumChildProperties);
		return (NumChildProperties == CustomizationProperties.Num());
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
			if (const UPulldownContents* PulldownContents = FPulldownBuilderUtils::FindPulldownContentsByStruct(StructProperty->Struct))
			{
				TArray<UObject*> OuterObjects;
				StructPropertyHandle->GetOuterObjects(OuterObjects);

				void* RawData;
				const FPropertyAccess::Result Result = StructPropertyHandle->GetValueData(RawData);
				if (Result != FPropertyAccess::Success)
				{
					RawData = nullptr;
				}
				
				FPulldownRows PulldownRows = PulldownContents->GetPulldownRows(
					OuterObjects,
					FStructContainer(StructProperty->Struct, static_cast<uint8*>(RawData))
				);
				if (PulldownContents->AllowNonExistentValue())
				{
					FName SelectedValue;
					FText DisplayName;
					if ((SelectedValueHandle->GetValue(SelectedValue) == FPropertyAccess::Success) &&
						(CachedDisplayTextHandle->GetValue(DisplayName) == FPropertyAccess::Success))
					{
						PulldownRows.SetNonExistentValue(SelectedValue, DisplayName);
					}
				}

				return PulldownRows;
			}
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
		check(SelectedValueHandle.IsValid());
		
		FName SelectedValue;
		SelectedValueHandle->GetValue(SelectedValue);
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
			SetPropertyValueSafe(CachedDisplayTextHandle.ToSharedRef(), SelectedItem->DisplayText);
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

	void FPulldownStructDetail::ApplyDefaultValue()
	{
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
		check(SelectedValueHandle.IsValid());
		
		FName SelectedValue;
		SelectedValueHandle->GetValue(SelectedValue);

		FPlatformApplicationMisc::ClipboardCopy(*SelectedValue.ToString());
	}

	void FPulldownStructDetail::OnSelectedValuePasteAction()
	{
		FName PastedSelectedValue;
		{
			FString ClipboardString;
			FPlatformApplicationMisc::ClipboardPaste(ClipboardString);
			if (ClipboardString.IsEmpty())
			{
				return;
			}
			
			PastedSelectedValue = *ClipboardString;
		}

		const TSharedPtr<FPulldownRow> SelectedItem = FindSelectableValueByName(PastedSelectedValue);
		OnSelectedValueChanged(SelectedItem, ESelectInfo::Direct);
		RefreshPulldownWidget();
	}
	
	void FPulldownStructDetail::OnResetToDefaultAction(TSharedPtr<IPropertyHandle> PropertyHandle)
	{
		ApplyDefaultValue();
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
