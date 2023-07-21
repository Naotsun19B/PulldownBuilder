﻿// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Widgets/SPulldownSelector.h"
#include "PulldownBuilder/Utilities/PulldownBuilderAppearanceSettings.h"
#include "PulldownBuilder/Types/PulldownRow.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/SToolTip.h"

namespace PulldownBuilder
{
	void SPulldownSelector::Construct(const FArguments& InArgs)
	{
		ListItemsSource = InArgs._ListItemsSource;
		OnSelectionChanged = InArgs._OnSelectionChanged;
		
		const auto& Settings = UPulldownBuilderAppearanceSettings::Get();
		
		ChildSlot
		[
			SNew(SBox)
			.HeightOverride(Settings.PanelSize.Y)
			.WidthOverride(Settings.PanelSize.X)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SSearchBox)
					.HintText(NSLOCTEXT("PulldownSelector", "SearchHintText", "Item Name or Tooltip Text"))
					.OnTextChanged(this, &SPulldownSelector::HandleOnTextChanged)
					.DelayChangeNotificationsWhileTyping(true)
				]
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				[
					SAssignNew(ListView, SListView<TSharedPtr<FPulldownRow>>)
					.SelectionMode(ESelectionMode::Single)
					.ListItemsSource(&ListItems)
					.OnGenerateRow(this, &SPulldownSelector::HandleOnGenerateRow)
					.OnMouseButtonClick(this, &SPulldownSelector::OnRowItemClicked)
					.OnMouseButtonDoubleClick(this, &SPulldownSelector::OnRowItemClicked)
				]
			]
		];
	}

	FReply SPulldownSelector::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
	{
		if (InKeyEvent.GetKey() == EKeys::Enter &&
			!InKeyEvent.IsAltDown() &&
			!InKeyEvent.IsCommandDown() &&
			!InKeyEvent.IsControlDown() &&
			!InKeyEvent.IsShiftDown())
		{
			const TSharedPtr<FPulldownRow> SelectedItem = GetSelectedItem();
			if (OnSelectionChanged.IsBound() && SelectedItem.IsValid())
			{
				OnSelectionChanged.Execute(SelectedItem, ESelectInfo::OnKeyPress);
				return FReply::Handled();
			}
		}

		return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
	}

	TSharedPtr<FPulldownRow> SPulldownSelector::GetSelectedItem() const
	{
		if (ListView.IsValid())
		{
			const TArray<TSharedPtr<FPulldownRow>> SelectedItems = ListView->GetSelectedItems();
			if (SelectedItems.IsValidIndex(0))
			{
				return SelectedItems[0];
			}
		}

		return nullptr;
	}

	void SPulldownSelector::SetSelectedItem(TSharedPtr<FPulldownRow> NewItem)
	{
		if (ListView.IsValid())
		{
			ListView->SetSelection(NewItem);
		}
	}

	void SPulldownSelector::RefreshList()
	{
		RebuildListItems();
		if (ListView.IsValid())
		{
			ListView->RebuildList();
		}
	}

	void SPulldownSelector::RebuildListItems()
	{
		if (ListItemsSource != nullptr)
		{
			ListItems = *ListItemsSource;
		}
	
		if (!FilterString.IsEmpty())
		{
			ListItems.RemoveAll([this](const TSharedPtr<FPulldownRow>& ListItem) -> bool
			{
				return !FilterRow(ListItem);
			});
		}
	}

	void SPulldownSelector::HandleOnTextChanged(const FText& InFilterString)
	{
		FilterString = InFilterString.ToString();
		RefreshList();
	}

	TSharedRef<ITableRow> SPulldownSelector::HandleOnGenerateRow(TSharedPtr<FPulldownRow> InItem, const TSharedRef<STableViewBase>& OwnerTable)
	{
		check(InItem.IsValid());

		const TSharedRef<STextBlock> Row = SNew(STextBlock)
			.Text(InItem->GetDisplayText())
			.HighlightText(FText::FromString(FilterString))
			.ColorAndOpacity(FSlateColor::UseForeground());

		if (!InItem->TooltipText.IsEmpty())
		{
			Row->SetToolTip(
				SNew(SToolTip)
				.Content()
				[
					SNew(STextBlock)
					.Text(InItem->TooltipText)
					.HighlightText(FText::FromString(FilterString))
				]
			);
		}

		return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		[
			Row
		];
	}

	void SPulldownSelector::HandleOnMouseButtonClick(TSharedPtr<FPulldownRow> SelectedItem)
	{
		if (!UPulldownBuilderAppearanceSettings::Get().bIsSelectWhenDoubleClick)
		{
			OnRowItemClicked(SelectedItem);
		}
	}

	void SPulldownSelector::HandleOnMouseButtonDoubleClick(TSharedPtr<FPulldownRow> SelectedItem)
	{
		if (UPulldownBuilderAppearanceSettings::Get().bIsSelectWhenDoubleClick)
		{
			OnRowItemClicked(SelectedItem);
		}
	}

	void SPulldownSelector::OnRowItemClicked(TSharedPtr<FPulldownRow> SelectedItem)
	{
		if (OnSelectionChanged.IsBound())
		{
			OnSelectionChanged.Execute(SelectedItem, ESelectInfo::OnMouseClick);
		}
	}

	bool SPulldownSelector::FilterRow(TSharedPtr<FPulldownRow> InListItem) const
	{
		check(InListItem.IsValid());

		const FString DisplayString = InListItem->GetDisplayText().ToString();
		const FString TooltipString = InListItem->TooltipText.ToString();
		return (
			DisplayString.Contains(FilterString) ||
			TooltipString.Contains(FilterString)
		);
	}
}
