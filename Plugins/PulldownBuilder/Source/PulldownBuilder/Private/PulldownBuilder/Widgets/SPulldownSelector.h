// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"

struct FPulldownRow;

namespace PulldownBuilder
{
	/**
	 * A custom list view widget that lists the items of a pull-down struct.
	 */
	class PULLDOWNBUILDER_API SPulldownSelector : public SCompoundWidget
	{
	public:
		// Define the event that will be called when a new item is selected.
		using FOnSelectionChanged = TSlateDelegates<TSharedPtr<FPulldownRow>>::FOnSelectionChanged;

	public:
		SLATE_BEGIN_ARGS(SPulldownSelector)
			: _ListItemsSource(nullptr)
			, _InitialSelection(nullptr)
		{}

		// A source data that is the basis of the items displayed in the pull-down menu.
		SLATE_ARGUMENT(const TArray<TSharedPtr<FPulldownRow>>*, ListItemsSource)
	
		// Sets the item that should be selected first.
		SLATE_ARGUMENT(TSharedPtr<FPulldownRow>, InitialSelection)

		// Called when a new item is selected.
		SLATE_EVENT(FOnSelectionChanged, OnSelectionChanged)
	
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs);

		// SWidget interface.
		virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
		// End of SWidget interface.
	
		// Getters and setters for the selected item.
		TSharedPtr<FPulldownRow> GetSelectedItem() const;
		void SetSelectedItem(TSharedPtr<FPulldownRow> NewItem);
	
		// Rebuilds the list.
		void RefreshList();

	private:
		// Rebuilds the underlying data of the list.
		void RebuildListItems();
	
		// Called when the text in the search field changes.
		void HandleOnTextChanged(const FText& InFilterString);

		// Called when a list row is generated.
		TSharedRef<ITableRow> HandleOnGenerateRow(TSharedPtr<FPulldownRow> InItem, const TSharedRef<STableViewBase>& OwnerTable);

		// Called when an item in the list is clicked.
		void HandleOnMouseButtonClick(TSharedPtr<FPulldownRow> SelectedItem);
		void HandleOnMouseButtonDoubleClick(TSharedPtr<FPulldownRow> SelectedItem);
		void OnRowItemClicked(TSharedPtr<FPulldownRow> SelectedItem);

		// Returns whether the items in the list match the filter string.
		bool FilterRow(TSharedPtr<FPulldownRow> InListItem) const;
	
	private:
		// A list view widget for internal use.
		TSharedPtr<SListView<TSharedPtr<FPulldownRow>>> ListView;

		// A pointer to the underlying data to use when building ListItems.
		const TArray<TSharedPtr<FPulldownRow>>* ListItemsSource = nullptr;
	
		// A list of items to display in the list.
		TArray<TSharedPtr<FPulldownRow>> ListItems;
	
		// A string by which to filter language codes.
		FString FilterString;
	
		// An event that is called when a new item is selected.
		FOnSelectionChanged OnSelectionChanged;
	};
}
