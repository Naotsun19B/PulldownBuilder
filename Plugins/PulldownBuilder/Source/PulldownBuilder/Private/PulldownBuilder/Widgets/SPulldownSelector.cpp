// Copyright 2021 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Widgets/SPulldownSelector.h"
#include "PulldownBuilder/Utilities/PulldownBuilderSettings.h"
#include "PulldownBuilder/Structs/PulldownRow.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/SToolTip.h"

void SPulldownSelector::Construct(const FArguments& InArgs)
{
	ListItemsSource = InArgs._ListItemsSource;
	OnSelectionChanged = InArgs._OnSelectionChanged;

	const auto* Settings = GetDefault<UPulldownBuilderSettings>();
	check(Settings);

	if (Settings->bIsSelectWhenDoubleClick)
	{
		ListView = SNew(SListView<TSharedPtr<FPulldownRow>>)
		.SelectionMode(ESelectionMode::Single)
		.ListItemsSource(ListItemsSource)
		.OnGenerateRow(this, &SPulldownSelector::HandleOnGenerateRow)
		.OnMouseButtonDoubleClick(this, &SPulldownSelector::OnRowItemClicked);
	}
	else
	{
		ListView = SNew(SListView<TSharedPtr<FPulldownRow>>)
		.SelectionMode(ESelectionMode::Single)
		.ListItemsSource(ListItemsSource)
		.OnGenerateRow(this, &SPulldownSelector::HandleOnGenerateRow)
		.OnMouseButtonClick(this, &SPulldownSelector::OnRowItemClicked);
	}

	ChildSlot
	[
		SNew(SBox)
		.HeightOverride(Settings->PanelSize.Y)
		.WidthOverride(Settings->PanelSize.X)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SSearchBox)
				.HintText(NSLOCTEXT("PulldownSelector", "SearchHintText", "Item Name or Tooltip Text"))
				.OnTextChanged(this, &SPulldownSelector::HandleOnTextChanged)
				.DelayChangeNotificationsWhileTyping(true)
			]
			+SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				ListView.ToSharedRef()
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
		ListView->RequestListRefresh();
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
		.Text(InItem->DisplayText)
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

	const FString DisplayString = InListItem->DisplayText.ToString();
	const FString TooltipString = InListItem->TooltipText.ToString();
	return (
		DisplayString.Contains(FilterString) ||
		TooltipString.Contains(FilterString)
	);
}
