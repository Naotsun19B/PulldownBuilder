// Copyright 2021-2025 Naotsun. All Rights Reserved.

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
		CachedPropertyTypeName = GetNameSafe(FNativeLessPulldownStruct::StaticStruct());
			
		auto& PropertyEditorModule = FPulldownBuilderUtils::GetPropertyEditorModule();
		PropertyEditorModule.RegisterCustomPropertyTypeLayout(
			*CachedPropertyTypeName,
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FNativeLessPulldownStructDetail::MakeInstance)
		);
	}

	void FNativeLessPulldownStructDetail::Unregister()
	{
		auto& PropertyEditorModule = FPulldownBuilderUtils::GetPropertyEditorModule();
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout(
			*CachedPropertyTypeName
		);
	}

	TSharedRef<IPropertyTypeCustomization> FNativeLessPulldownStructDetail::MakeInstance()
	{
		return MakeShared<FNativeLessPulldownStructDetail>();
	}

	void FNativeLessPulldownStructDetail::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
	{
		// Sets the handles for properties that need to be customized.
		CustomizationProperties.Add(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource), &PulldownSourceHandle);
		
		FPulldownStructDetail::CustomizeHeader(InStructPropertyHandle, HeaderRow, StructCustomizationUtils);
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

	FPulldownRows FNativeLessPulldownStructDetail::GenerateSelectableValues()
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
					MakeShared<FPulldownRow>(
						PulldownContents->GetName(),
						FText::FromString(PulldownContents->GetTooltip())
					)
				);
			}
		}

		FName PulldownSource;
		PulldownSourceHandle->GetValue(PulldownSource);
		if (const UPulldownContents* SourceAsset = FPulldownBuilderUtils::FindPulldownContentsByName(PulldownSource))
		{
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
			
				return SourceAsset->GetPulldownRows(
					OuterObjects,
					FStructContainer(StructProperty->Struct, static_cast<uint8*>(RawData))
				);
			}
		}

		return FPulldownRows::Empty;
	}

	void FNativeLessPulldownStructDetail::OnMultipleSelected()
	{
		PulldownContentsNames.Reset();
		FPulldownStructDetail::OnMultipleSelected();
	}

	UPulldownContents* FNativeLessPulldownStructDetail::GetRelatedPulldownContents() const
	{
		check(PulldownSourceHandle.IsValid());

		FName PulldownSource;
		PulldownSourceHandle->GetValue(PulldownSource);
		return FPulldownBuilderUtils::FindPulldownContentsByName(PulldownSource);
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
					.HeightOverride(this, &FNativeLessPulldownStructDetail::GetIndividualPanelHeight)
					.WidthOverride(this, &FNativeLessPulldownStructDetail::GetIndividualPanelWidth)
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
		return PulldownContentsNames.FindByPredicate(
			[&](const TSharedPtr<FPulldownRow>& Row)
			{
				return (Row.IsValid() && Row->SelectedValue.Equals(InName.ToString()));
			}
		);
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

		const FName NewPulldownSource = *SelectedItem->SelectedValue;
		FName OldPulldownSource;
		PulldownSourceHandle->GetValue(OldPulldownSource);
		if (NewPulldownSource != OldPulldownSource)
		{
			PulldownSourceHandle->SetValue(NewPulldownSource);

			// Since the base asset of the pull-down menu has changed, set SelectedValue to None.
			SelectedValueHandle->SetValue(FName(NAME_None));

			UpdateSearchableObject();
			ApplyDefaultValue();
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

	FString FNativeLessPulldownStructDetail::CachedPropertyTypeName;
}
