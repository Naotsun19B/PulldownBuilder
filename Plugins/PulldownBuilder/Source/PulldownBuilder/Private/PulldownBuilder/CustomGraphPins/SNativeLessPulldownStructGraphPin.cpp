// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/CustomGraphPins/SNativeLessPulldownStructGraphPin.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownBuilder/Widgets/SPulldownSelectorComboButton.h"
#include "PulldownBuilder/Types/PulldownRow.h"
#include "PulldownBuilder/Types/StructContainer.h"
#include "PulldownStruct/NativeLessPulldownStruct.h"

namespace PulldownBuilder
{
	void SNativeLessPulldownStructGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
	{
		SPulldownStructGraphPin::Construct(SPulldownStructGraphPin::FArguments(), InGraphPinObj);
	}

	TSharedRef<SWidget> SNativeLessPulldownStructGraphPin::GetDefaultValueWidget()
	{
		InitializePulldown();
		
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
					.GetSelection(this, &SNativeLessPulldownStructGraphPin::GetPulldownSourceSelection)
					.HeightOverride(this, &SNativeLessPulldownStructGraphPin::GetIndividualPanelHeight)
					.WidthOverride(this, &SNativeLessPulldownStructGraphPin::GetIndividualPanelWidth)
					.OnSelectionChanged(this, &SNativeLessPulldownStructGraphPin::OnPulldownSourceChanged)
					.OnComboBoxOpened(this, &SNativeLessPulldownStructGraphPin::RebuildPulldown)
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
		TSharedPtr<FPulldownRow> SelectedItem = nullptr;

		FName CurrentPulldownSource;
		if (GetPropertyValue(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource), CurrentPulldownSource))
		{
			SelectedItem = FindPulldownContentsNameByName(CurrentPulldownSource);
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

	FPulldownRows SNativeLessPulldownStructGraphPin::GenerateSelectableValues()
	{
		check(GraphPinObj != nullptr);
		
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
		if (GetPropertyValue(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource), PulldownSource))
		{
			if (const UPulldownContents* SourceAsset = FPulldownBuilderUtils::FindPulldownContentsByName(PulldownSource))
			{
				FStructContainer StructContainer;
				if (FPulldownBuilderUtils::GenerateStructContainerFromPin(GraphPinObj, StructContainer))
				{
					return SourceAsset->GetPulldownRows(
						TArray<UObject*>{ FPulldownBuilderUtils::GetOuterAssetFromPin(GraphPinObj) },
						StructContainer
					);
				}
			}
		}

		return FPulldownRows::Empty;
	}

	UPulldownContents* SNativeLessPulldownStructGraphPin::GetRelatedPulldownContents() const
	{
		FName SearchableObject;
		if (!GetPropertyValue(FPulldownStructBase::SearchableObjectPropertyName, SearchableObject))
		{
			return nullptr;
		}
		
		return FPulldownBuilderUtils::FindPulldownContentsByName(SearchableObject);
	}

	TSharedPtr<FPulldownRow> SNativeLessPulldownStructGraphPin::FindPulldownContentsNameByName(const FName& InName) const
	{
		return PulldownContentsNames.FindByPredicate(
			[&](const TSharedPtr<FPulldownRow>& Row)
			{
				return (Row.IsValid() && Row->SelectedValue.Equals(InName.ToString()));
			}
		);
	}

	void SNativeLessPulldownStructGraphPin::OnPulldownSourceChanged(TSharedPtr<FPulldownRow> SelectedItem, ESelectInfo::Type SelectInfo)
	{
		if (!SelectedItem.IsValid())
		{
			return;
		}

		FName CurrentPulldownSource;
		if (!GetPropertyValue(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource), CurrentPulldownSource))
		{
			return;
		}

		if (*SelectedItem->SelectedValue == CurrentPulldownSource)
		{
			return;
		}
		
		SetPropertyValue(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource), SelectedItem->SelectedValue);

		// Since the base asset of the pull-down menu has changed, sets SelectedValue to None.
		SetPropertyValue(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue), NAME_None);
		InitializePulldown();
	}

	TSharedPtr<FPulldownRow> SNativeLessPulldownStructGraphPin::GetPulldownSourceSelection() const
	{
		FName PulldownSource;
		if (!GetPropertyValue(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource), PulldownSource))
		{
			return nullptr;
		}
		
		return FindPulldownContentsNameByName(PulldownSource);
	}
}
