// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Widgets/SPulldownSelectorComboButton.h"
#include "PulldownBuilder/Types/PulldownRow.h"

#define LOCTEXT_NAMESPACE "PulldownSelectorComboButton"

namespace PulldownBuilder
{
	void SPulldownSelectorComboButton::Construct(const FArguments& InArgs)
	{
		ListItemsSource = InArgs._ListItemsSource;
		GetSelection = InArgs._GetSelection;
		OnSelectionChanged = InArgs._OnSelectionChanged;

		check(GetSelection.IsBound());
		SelectedPulldownRow = GetSelection.Execute();
		
		SComboButton::Construct(
			SComboButton::FArguments()
			.ContentPadding(FMargin(2, 2, 2, 1))
			.MenuPlacement(MenuPlacement_BelowAnchor)
			.OnComboBoxOpened(InArgs._OnComboBoxOpened)
			.OnGetMenuContent(this, &SPulldownSelectorComboButton::HandleOnGetMenuContent)
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text(this, &SPulldownSelectorComboButton::GetDisplayText)
				.ToolTipText(this, &SPulldownSelectorComboButton::GetTooltipText)
			]
		);
	}

	void SPulldownSelectorComboButton::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
	{
		SComboButton::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

		// In certain environments the initial value may not be passed from the pin, so if there is no item selected as insurance, it will be reacquired.
		if (!SelectedPulldownRow.IsValid())
		{
			SelectedPulldownRow = GetSelection.Execute();
		}
	}

	TSharedPtr<FPulldownRow> SPulldownSelectorComboButton::GetSelectedItem() const
	{
		return SelectedPulldownRow;
	}

	void SPulldownSelectorComboButton::SetSelectedItem(TSharedPtr<FPulldownRow> NewItem)
	{
		SelectedPulldownRow = NewItem;
	}

	void SPulldownSelectorComboButton::RefreshList()
	{
		if (PulldownSelector.IsValid())
		{
			PulldownSelector->RefreshList();
		}
	}

	TSharedRef<SWidget> SPulldownSelectorComboButton::HandleOnGetMenuContent()
	{
		return
			SAssignNew(PulldownSelector, SPulldownSelector)
			.ListItemsSource(ListItemsSource)
			.InitialSelection(SelectedPulldownRow)
			.OnSelectionChanged(this, &SPulldownSelectorComboButton::HandleOnSelectionChanged);
	}

	FText SPulldownSelectorComboButton::GetDisplayText() const
	{
		const TSharedPtr<FPulldownRow> SelectedItem = GetSelectedItem();
		if (SelectedItem.IsValid())
		{
			return SelectedItem->GetDisplayText();
		}
		
		return LOCTEXT("InvalidDisplayText", "Invalid Data");
	}

	FText SPulldownSelectorComboButton::GetTooltipText() const
	{
		const TSharedPtr<FPulldownRow> SelectedItem = GetSelectedItem();
		if (SelectedItem.IsValid())
		{
			return SelectedItem->TooltipText;
		}

		return LOCTEXT("InvalidTooltipText", "Invalid Data");
	}

	void SPulldownSelectorComboButton::HandleOnSelectionChanged(TSharedPtr<FPulldownRow> SelectedItem, ESelectInfo::Type SelectInfo)
	{
		if (OnSelectionChanged.IsBound())
		{
			OnSelectionChanged.Execute(SelectedItem, SelectInfo);
		}

		if (SelectedItem.IsValid())
		{
			SelectedPulldownRow = SelectedItem;
		}

		SetIsOpen(false);
		PulldownSelector.Reset();
	}
}

#undef LOCTEXT_NAMESPACE
