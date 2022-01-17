// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownBuilder/CustomGraphPins/SNativeLessPulldownStructGraphPin.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownBuilder/Widgets/SPulldownSelectorComboButton.h"
#include "PulldownBuilder/Structs/PulldownRow.h"
#include "PulldownStruct/NativeLessPulldownStruct.h"

namespace PulldownBuilder
{
	void SNativeLessPulldownStructGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
	{
		SPulldownStructGraphPin::Construct(SPulldownStructGraphPin::FArguments(), InGraphPinObj);
	}

	TSharedRef<SWidget> SNativeLessPulldownStructGraphPin::GetDefaultValueWidget()
	{
		RebuildPulldown();

		const TSharedPtr<FName> PulldownSource = GetPropertyValue(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource));
		const FName& NameToFind = (PulldownSource.IsValid() ? *PulldownSource : NAME_None);
	
		return
			SNew(SHorizontalBox)
			.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SHorizontalBox)
				.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(PulldownSourceWidget, SPulldownSelectorComboButton)
					.ListItemsSource(&PulldownContentsNames)
					.OnSelectionChanged(this, &SNativeLessPulldownStructGraphPin::OnPulldownSourceChanged)
					.OnComboBoxOpened(this, &SNativeLessPulldownStructGraphPin::RebuildPulldown)
					.InitialSelection(FindPulldownContentsNameByName(NameToFind))
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				GenerateSelectableValuesWidget()
			];
	}

	void SNativeLessPulldownStructGraphPin::RefreshPulldownWidget()
	{
		// Check if the currently set string is included in the constructed list.
		const TSharedPtr<FName>& CurrentPulldownSource = GetPropertyValue(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource));
		TSharedPtr<FPulldownRow> SelectedItem = nullptr;
		if (CurrentPulldownSource.IsValid())
		{
			SelectedItem = FindPulldownContentsNameByName(*CurrentPulldownSource);
		}
		if (!SelectedItem.IsValid())
		{
			SetPropertyValue(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource), NAME_None);
			SelectedItem = FindPulldownContentsNameByName(NAME_None);
		}

		if (PulldownSourceWidget.IsValid())
		{
			PulldownSourceWidget->RefreshList();
			PulldownSourceWidget->SetSelectedItem(SelectedItem);
		}

		SPulldownStructGraphPin::RefreshPulldownWidget();
	}

	TArray<TSharedPtr<FPulldownRow>> SNativeLessPulldownStructGraphPin::GenerateSelectableValues()
	{
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

		const TSharedPtr<FName> PulldownSource = GetPropertyValue(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource));
		if (PulldownSource.IsValid())
		{
			if (const UPulldownContents* SourceAsset = FPulldownBuilderUtils::FindPulldownContentsByName(*PulldownSource))
			{
				return SourceAsset->GetPulldownRows();
			}
		}

		return FPulldownBuilderUtils::GetEmptyPulldownRows();
	}

	TSharedPtr<FPulldownRow> SNativeLessPulldownStructGraphPin::FindPulldownContentsNameByName(const FName& InName) const
	{
		const TSharedPtr<FPulldownRow>* FoundItem = PulldownContentsNames.FindByPredicate(
			[&](const TSharedPtr<FPulldownRow>& Item)
			{
				return (Item.IsValid() && Item->DisplayText.ToString() == InName.ToString());
			});

		return (FoundItem != nullptr ? *FoundItem : nullptr);
	}

	void SNativeLessPulldownStructGraphPin::OnPulldownSourceChanged(TSharedPtr<FPulldownRow> SelectedItem, ESelectInfo::Type SelectInfo)
	{
		const TSharedPtr<FName> CurrentPulldownSource = GetPropertyValue(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource));
		if (SelectedItem.IsValid() && CurrentPulldownSource.IsValid())
		{
			if (*SelectedItem != CurrentPulldownSource->ToString())
			{
				SetPropertyValue(
					GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource),
					*SelectedItem->DisplayText.ToString()
				);

				// Since the base asset of the pull-down menu has changed, set SelectedValue to None.
				SetPropertyValue(
					GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue),
					NAME_None
				);
				SPulldownStructGraphPin::RefreshPulldownWidget();
			}	
		}
	}
}
