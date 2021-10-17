// Copyright 2021 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Widgets/SPulldownSelectorComboButton.h"
#include "PulldownBuilder/Structs/PulldownRow.h"

#define LOCTEXT_NAMESPACE "PulldownSelectorComboButton"

void SPulldownSelectorComboButton::Construct(const FArguments& InArgs)
{
	ListItemsSource = InArgs._ListItemsSource;
	SelectedPulldownRow = InArgs._InitialSelection;
	OnSelectionChanged = InArgs._OnSelectionChanged;
	
	SComboButton::Construct(
		SComboButton::FArguments()
		.ContentPadding(FMargin(2, 2, 2, 1))
		.MenuPlacement(EMenuPlacement::MenuPlacement_BelowAnchor)
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

TSharedPtr<FPulldownRow> SPulldownSelectorComboButton::GetSelectedItem() const
{
	return SelectedPulldownRow;
}

void SPulldownSelectorComboButton::SetSelectedItem(TSharedPtr<FPulldownRow> NewItem)
{
	SelectedPulldownRow = NewItem;
}

TSharedRef<SWidget> SPulldownSelectorComboButton::HandleOnGetMenuContent()
{
	return
		SNew(SPulldownSelector)
		.ListItemsSource(ListItemsSource)
		.InitialSelection(SelectedPulldownRow)
		.OnSelectionChanged(this, &SPulldownSelectorComboButton::HandleOnSelectionChanged);
}

FText SPulldownSelectorComboButton::GetDisplayText() const
{
	const TSharedPtr<FPulldownRow> SelectedItem = GetSelectedItem();
	if (SelectedItem.IsValid())
	{
		return SelectedItem->DisplayText;
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
}

#undef LOCTEXT_NAMESPACE
