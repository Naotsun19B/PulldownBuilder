// Copyright 2021-2023 Naotsun. All Rights Reserved.

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
#include "PropertyHandle.h"
#include "IDetailChildrenBuilder.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Modules/ModuleManager.h"
#if UE_5_01_OR_LATER
#include "Styling/AppStyle.h"
#else
#include "EditorStyleSet.h"
#endif

#define LOCTEXT_NAMESPACE "PulldownStructDetail"

namespace PulldownBuilder
{
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

		// Scan the properties of the struct for the property handle of FPulldownStructBase::SelectedValue.
		uint32 NumChildProperties;
		StructPropertyHandle->GetNumChildren(NumChildProperties);
		for (uint32 Index = 0; Index < NumChildProperties; Index++)
		{
			const TSharedPtr<IPropertyHandle> ChildPropertyHandle = StructPropertyHandle->GetChildHandle(Index);
			if (ChildPropertyHandle.IsValid())
			{
#if UE_4_25_OR_LATER
				if (const FProperty* ChildProperty = ChildPropertyHandle->GetProperty())
#else
				if (const UProperty* ChildProperty = ChildPropertyHandle->GetProperty())
#endif
				{
					if (ChildProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue))
					{
						SelectedValueHandle = ChildPropertyHandle;
						break;
					}
				}
			}
		}

		// Do not register structures other than FPulldownStructBase in this detail customization.
		check(SelectedValueHandle.IsValid());
	
		HeaderRow.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		];

		// If the property is only FPulldownStructBase::SelectedValue, display it inline.
		if (NumChildProperties == 1 && UPulldownBuilderAppearanceSettings::Get().bShouldInlineDisplayWhenSingleProperty)
		{
			HeaderRow.ValueContent()
				.MinDesiredWidth(500)
				[
					GenerateSelectableValuesWidget()
				];

			HeaderRow.CopyAction(CreateSelectedValueCopyAction());
			HeaderRow.PasteAction(CreateSelectedValuePasteAction());
			AddBrowseSourceAssetAction(HeaderRow);
			
			RebuildPulldown();
		}
	}

	void FPulldownStructDetail::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
	{
		check(StructPropertyHandle.IsValid() && SelectedValueHandle.IsValid());

		// Add child properties other than FPulldownStructBase::SelectedValue to the StructBuilder.
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
					if (!IsCustomizationTarget(ChildProperty))
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
		if (NumChildProperties > 1 || !UPulldownBuilderAppearanceSettings::Get().bShouldInlineDisplayWhenSingleProperty)
		{
			AddCustomRowBeforeSelectedValue(StructBuilder);
			
			FDetailWidgetRow& DetailWidgetRow = StructBuilder.AddCustomRow(
				FText::FromName(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue))
			)
				.CopyAction(CreateSelectedValueCopyAction())
				.PasteAction(CreateSelectedValuePasteAction())
				.NameContent()
				[
					SelectedValueHandle->CreatePropertyNameWidget()
				]
				.ValueContent()
				.MinDesiredWidth(500)
				[
					GenerateSelectableValuesWidget()
				];

			AddBrowseSourceAssetAction(DetailWidgetRow);
			
			AddCustomRowAfterSelectedValue(StructBuilder);

			RebuildPulldown();
		}
	}

	void FPulldownStructDetail::RebuildPulldown()
	{
		check(StructPropertyHandle.IsValid() && SelectedValueHandle.IsValid());

		// Find PulldownContents in the property struct and
		// build a list of strings to display in the pull-down menu.
		void* StructValueData = nullptr;
		const FPropertyAccess::Result Result = StructPropertyHandle->GetValueData(StructValueData);
		if (Result == FPropertyAccess::Success)
		{	
			SelectableValues = GenerateSelectableValues();
		}
		// Empty the list if data acquisition fails or if multiple selections are made.
		else
		{
			OnMultipleSelected();
			return;
		}

		RefreshPulldownWidget();
	}

	void FPulldownStructDetail::RefreshPulldownWidget()
	{
		check(SelectedValueHandle.IsValid());
	
		// Check if the currently set string is included in the constructed list.
		FName CurrentSelectedValue;
		SelectedValueHandle->GetValue(CurrentSelectedValue);

		TSharedPtr<FPulldownRow> SelectedItem = FindSelectableValueByName(CurrentSelectedValue);
		if (!SelectedItem.IsValid())
		{
			// There are parts of the MovieScene module code that don't check for nullptr.
			// This causes a crash when changing properties in the sequencer editor, so until this is fixed,
			// for MovieSceneSignedObject (ex: sections or tracks), auto-reset to None at the next frame.
			auto IsMovieSceneClass = [](const UClass* TestClass) -> bool
			{
				const UClass* Class = TestClass;
				while (IsValid(Class))
				{
					if (Class->GetName() == TEXT("MovieSceneSignedObject"))
					{
						return true;
					}

					Class = Class->GetSuperClass();
				}

				return false;
			};
			
			const UClass* OuterBaseClass = SelectedValueHandle->GetOuterBaseClass();
			if (IsMovieSceneClass(OuterBaseClass))
			{
				check(IsValid(GEditor));
				TWeakPtr<IPropertyHandle> WeakSelectedValueHandle = SelectedValueHandle;
				GEditor->GetTimerManager()->SetTimerForNextTick(
					[WeakSelectedValueHandle]()
					{
						if (WeakSelectedValueHandle.IsValid())
						{
							WeakSelectedValueHandle.Pin()->SetValue(NAME_None);
						}
					}
				);
			}
			else
			{
				SelectedValueHandle->SetValue(NAME_None);
			}
			SelectedItem = FindSelectableValueByName(NAME_None);
		}

		if (SelectedValueWidget.IsValid())
		{
			SelectedValueWidget->RefreshList();
			SelectedValueWidget->SetSelectedItem(SelectedItem);
		}
	}

	TArray<TSharedPtr<FPulldownRow>> FPulldownStructDetail::GenerateSelectableValues()
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

		return FPulldownBuilderUtils::GetEmptyPulldownRows();
	}

	void FPulldownStructDetail::OnMultipleSelected()
	{
		SelectableValues.Reset();
	}

#if UE_4_25_OR_LATER
	bool FPulldownStructDetail::IsCustomizationTarget(FProperty* InProperty) const
#else
	bool FPulldownStructDetail::IsCustomizationTarget(UProperty* InProperty) const
#endif
	{
		check(InProperty != nullptr);
		return (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue));
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

	TSharedPtr<FPulldownRow> FPulldownStructDetail::FindSelectableValueByName(const FName& InName) const
	{
		const TSharedPtr<FPulldownRow>* FoundItem = SelectableValues.FindByPredicate(
			[&](const TSharedPtr<FPulldownRow>& Item)
			{
				return (Item.IsValid() && Item->SelectedValue == InName.ToString());
			});

		return (FoundItem != nullptr ? *FoundItem : nullptr);
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

	UPulldownContents* FPulldownStructDetail::GetRelatedPulldownContents() const
	{
		check(StructPropertyHandle.IsValid());
		
#if UE_4_25_OR_LATER
		if (const auto* StructProperty = CastField<FStructProperty>(StructPropertyHandle->GetProperty()))
#else
		if (const auto* StructProperty = Cast<UStructProperty>(StructPropertyHandle->GetProperty()))
#endif
		{
			return FPulldownBuilderUtils::FindPulldownContentsByStruct(StructProperty->Struct);
		}

		return nullptr;
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
		if (!SelectedItem.IsValid() || !SelectedValueHandle.IsValid())
		{
			return;
		}

		const FName NewSelectedValue = *SelectedItem->SelectedValue;
		FName OldSelectedValue;
		SelectedValueHandle->GetValue(OldSelectedValue);
		if (NewSelectedValue != OldSelectedValue)
		{
			SelectedValueHandle->SetValue(NewSelectedValue);
		}
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

	FUIAction FPulldownStructDetail::CreateBrowseSourceAssetAction()
	{
		return FUIAction
		(
			FExecuteAction::CreateSP(this, &FPulldownStructDetail::OnBrowseSourceAssetAction),
			FCanExecuteAction::CreateSP(this, &FPulldownStructDetail::CanBrowseSourceAssetAction)
		);
	}

	void FPulldownStructDetail::AddBrowseSourceAssetAction(FDetailWidgetRow& DetailWidgetRow)
	{
		DetailWidgetRow.AddCustomContextMenuAction(
			CreateBrowseSourceAssetAction(),
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

	bool FPulldownStructDetail::CanBrowseSourceAssetAction() const
	{
		return IsValid(GetRelatedPulldownContents());
	}
}

#undef LOCTEXT_NAMESPACE
