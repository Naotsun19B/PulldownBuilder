// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownBuilder/DetailCustomizations/NativeLessPulldownStructDetail.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownBuilder/Widgets/SPulldownSelectorComboButton.h"
#include "PulldownBuilder/Types/PulldownRow.h"
#include "PulldownBuilder/Types/StructContainer.h"
#include "PulldownStruct/NativeLessPulldownStruct.h"
#include "DetailWidgetRow.h"
#include "PropertyEditorModule.h"
#include "PropertyHandle.h"
#include "IDetailChildrenBuilder.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Modules/ModuleManager.h"

namespace PulldownBuilder
{
	void FNativeLessPulldownStructDetail::Register()
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomPropertyTypeLayout(
			*GetNameSafe(FNativeLessPulldownStruct::StaticStruct()),
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FNativeLessPulldownStructDetail::MakeInstance)
		);
	}

	void FNativeLessPulldownStructDetail::Unregister()
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout(
			*GetNameSafe(FNativeLessPulldownStruct::StaticStruct())
		);
	}

	TSharedRef<IPropertyTypeCustomization> FNativeLessPulldownStructDetail::MakeInstance()
	{
		return MakeShared<FNativeLessPulldownStructDetail>();
	}

	void FNativeLessPulldownStructDetail::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
	{
		FPulldownStructDetail::CustomizeHeader(InStructPropertyHandle, HeaderRow, StructCustomizationUtils);

		// Scan the properties of the structure for the property handle of FNativeLessPulldownStruct::PulldownContentsName.
		uint32 NumChildProperties;
		StructPropertyHandle->GetNumChildren(NumChildProperties);
		for (uint32 Index = 0; Index < NumChildProperties; Index++)
		{
			const TSharedPtr<IPropertyHandle> ChildPropertyHandle = StructPropertyHandle->GetChildHandle(Index);
			if (ChildPropertyHandle.IsValid())
			{
#if BEFORE_UE_4_24
				if (const UProperty* ChildProperty = ChildPropertyHandle->GetProperty())
#else
				if (const FProperty* ChildProperty = ChildPropertyHandle->GetProperty())
#endif
				{
					if (ChildProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource))
					{
						PulldownSourceHandle = ChildPropertyHandle;
					}
				}
			}
		}

		// Do not register structures other than FNativeLessPulldownStruct in this detail customization.
		check(PulldownSourceHandle.IsValid());
	}

	void FNativeLessPulldownStructDetail::RefreshPulldownWidget()
	{
		check(PulldownSourceHandle.IsValid());
	
		// Check if the currently set string is included in the constructed list.
		FName CurrentPulldownSource;
		PulldownSourceHandle->GetValue(CurrentPulldownSource);

		TSharedPtr<FPulldownRow> SelectedItem = FindPulldownContentsNameByName(CurrentPulldownSource);
		if (!SelectedItem.IsValid())
		{
			PulldownSourceHandle->SetValue(NAME_None);
			SelectedItem = FindPulldownContentsNameByName(NAME_None);
		}

		if (PulldownSourceWidget.IsValid())
		{
			PulldownSourceWidget->RefreshList();
			PulldownSourceWidget->SetSelectedItem(SelectedItem);
		}

		FPulldownStructDetail::RefreshPulldownWidget();
	}

	TArray<TSharedPtr<FPulldownRow>> FNativeLessPulldownStructDetail::GenerateSelectableValues()
	{
		check(StructPropertyHandle.IsValid());
		
		PulldownContentsNames.Reset();
		PulldownContentsNames.Add(MakeShared<FPulldownRow>());

		const TArray<UPulldownContents*>& AllPulldownContents = FPulldownBuilderUtils::GetAllPulldownContents();
		for (const auto& PulldownContents : AllPulldownContents)
		{
			if (IsValid(PulldownContents))
			{
				PulldownContentsNames.Add(
					MakeShared<FPulldownRow>(PulldownContents->GetName(), PulldownContents->GetTooltip())
				);
			}
		}

		FName PulldownSource;
		PulldownSourceHandle->GetValue(PulldownSource);
		if (const UPulldownContents* SourceAsset = FPulldownBuilderUtils::FindPulldownContentsByName(PulldownSource))
		{
#if BEFORE_UE_4_24
			if (const auto* StructProperty = Cast<UStructProperty>(StructPropertyHandle->GetProperty()))
#else
			if (const auto* StructProperty = CastField<FStructProperty>(StructPropertyHandle->GetProperty()))
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
			
				return SourceAsset->GetPulldownRows(
					OuterObjects,
					FStructContainer(StructProperty->Struct, static_cast<uint8*>(RawData))
				);
			}
		}

		return FPulldownBuilderUtils::GetEmptyPulldownRows();
	}

	void FNativeLessPulldownStructDetail::OnMultipleSelected()
	{
		PulldownContentsNames.Reset();
		FPulldownStructDetail::OnMultipleSelected();
	}

#if BEFORE_UE_4_24
	bool FNativeLessPulldownStructDetail::IsCustomizationTarget(UProperty* InProperty) const
#else
	bool FNativeLessPulldownStructDetail::IsCustomizationTarget(FProperty* InProperty) const
#endif
	{
		check(InProperty != nullptr);
		
		return (
			FPulldownStructDetail::IsCustomizationTarget(InProperty) ||
			InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource)
		);
	}

	void FNativeLessPulldownStructDetail::AddCustomRowBeforeSelectedValue(IDetailChildrenBuilder& StructBuilder)
	{
		StructBuilder.AddCustomRow(FText::FromName(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource)))
			.CopyAction(CreatePulldownSourceCopyAction())
			.PasteAction(CreatePulldownSourcePasteAction())
			.NameContent()
			[
				PulldownSourceHandle->CreatePropertyNameWidget()
			]
			.ValueContent()
			.MinDesiredWidth(500)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				[
					SAssignNew(PulldownSourceWidget, SPulldownSelectorComboButton)
					.ListItemsSource(&PulldownContentsNames)
					.GetSelection(this, &FNativeLessPulldownStructDetail::GetPulldownSourceSelection)
					.OnSelectionChanged(this, &FNativeLessPulldownStructDetail::OnPulldownSourceChanged)
					.OnComboBoxOpened(this, &FNativeLessPulldownStructDetail::RebuildPulldown)
				]
			];
	}

	void FNativeLessPulldownStructDetail::OnBrowseSourceAssetAction()
	{
		check(PulldownSourceHandle.IsValid());
		
		FName PulldownSource;
		PulldownSourceHandle->GetValue(PulldownSource);
		if (UPulldownContents* SourceAsset = FPulldownBuilderUtils::FindPulldownContentsByName(PulldownSource))
		{
			FPulldownBuilderUtils::OpenPulldownContents(SourceAsset);
		}
	}

	bool FNativeLessPulldownStructDetail::CanBrowseSourceAssetAction() const
	{
		check(PulldownSourceHandle.IsValid());
		
		FName PulldownSource;
		PulldownSourceHandle->GetValue(PulldownSource);
		return IsValid(FPulldownBuilderUtils::FindPulldownContentsByName(PulldownSource));
	}

	TSharedPtr<FPulldownRow> FNativeLessPulldownStructDetail::FindPulldownContentsNameByName(const FName& InName) const
	{
		const TSharedPtr<FPulldownRow>* FoundItem = PulldownContentsNames.FindByPredicate(
			[&](const TSharedPtr<FPulldownRow>& Item)
			{
				return (Item.IsValid() && Item->DisplayText.ToString() == InName.ToString());
			});

		return (FoundItem != nullptr ? *FoundItem : nullptr);
	}

	TSharedPtr<FPulldownRow> FNativeLessPulldownStructDetail::GetPulldownSourceSelection() const
	{
		check(PulldownSourceHandle.IsValid());
		
		FName SelectedValue = NAME_None;
		if (PulldownSourceHandle.IsValid())
		{
			PulldownSourceHandle->GetValue(SelectedValue);
		}

		return FindPulldownContentsNameByName(SelectedValue);
	}

	void FNativeLessPulldownStructDetail::OnPulldownSourceChanged(TSharedPtr<FPulldownRow> SelectedItem, ESelectInfo::Type SelectInfo)
	{
		check(PulldownSourceHandle.IsValid() && SelectedValueHandle.IsValid());
	
		if (!SelectedItem.IsValid())
		{
			return;
		}

		const FName NewPulldownSource = *SelectedItem->DisplayText.ToString();
		FName OldPulldownSource;
		PulldownSourceHandle->GetValue(OldPulldownSource);
		if (NewPulldownSource != OldPulldownSource)
		{
			PulldownSourceHandle->SetValue(NewPulldownSource);

			// Since the base asset of the pull-down menu has changed, set SelectedValue to None.
			SelectedValueHandle->SetValue(FName(NAME_None));
		
			FPulldownStructDetail::RefreshPulldownWidget();
		}
	}

	FUIAction FNativeLessPulldownStructDetail::CreatePulldownSourceCopyAction()
	{
		return FUIAction
		(
			FExecuteAction::CreateSP(this, &FNativeLessPulldownStructDetail::OnPulldownSourceCopyAction)
		);
	}

	FUIAction FNativeLessPulldownStructDetail::CreatePulldownSourcePasteAction()
	{
		return FUIAction
		(
			FExecuteAction::CreateSP(this, & FNativeLessPulldownStructDetail::OnPulldownSourcePasteAction)
		);
	}

	void FNativeLessPulldownStructDetail::OnPulldownSourceCopyAction()
	{
		check(PulldownSourceHandle.IsValid());

		FName SelectedValue;
		PulldownSourceHandle->GetValue(SelectedValue);

		FPlatformApplicationMisc::ClipboardCopy(*SelectedValue.ToString());
	}

	void FNativeLessPulldownStructDetail::OnPulldownSourcePasteAction()
	{
		check(PulldownSourceWidget.IsValid());
	
		FName PastedText;
		{
			FString ClipboardString;
			FPlatformApplicationMisc::ClipboardPaste(ClipboardString);
			PastedText = *ClipboardString;
		}

		const TSharedPtr<FPulldownRow> SelectedItem = FindPulldownContentsNameByName(PastedText);
		if (SelectedItem.IsValid())
		{
			PulldownSourceWidget->SetSelectedItem(SelectedItem);
		}
	}
}
