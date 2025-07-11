// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"

struct FPulldownRow;
class FUICommandList;
class SSearchBox;

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
		{}

		// A source data that is the basis of the items displayed in the pull-down menu.
		SLATE_ARGUMENT(const TArray<TSharedPtr<FPulldownRow>>*, ListItemsSource)

		// Sets the vertical size of this widget.
		SLATE_ARGUMENT(float, HeightOverride)

		// Sets the horizontal size of this widget.
		SLATE_ARGUMENT(float, WidthOverride)

		// Whether you need to double-click to select an item.
		SLATE_ARGUMENT(bool, bIsSelectWhenDoubleClick)
		
		// Called when a new item is selected.
		SLATE_EVENT(FOnSelectionChanged, OnSelectionChanged)
	
		SLATE_END_ARGS()

		// Constructor.
		void Construct(const FArguments& InArgs);

		// SWidget interface.
		virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
		virtual bool SupportsKeyboardFocus() const override;
		// End of SWidget interface.
	
		// Getters and setters for the selected item.
		TSharedPtr<FPulldownRow> GetSelectedItem() const;
		void SetSelectedItem(const TSharedPtr<FPulldownRow>& NewItem);
	
		// Rebuilds the list.
		void RefreshList();

		// Returns the search box widget for internal use.
		TSharedPtr<SWidget> GetSearchBox() const;

	private:
		// Rebuilds the underlying data of the list.
		void RebuildListItems();
	
		// Called when the text in the search field changes.
		void HandleOnTextChanged(const FText& InFilterString);

		// Called when a list row is generated.
		TSharedRef<ITableRow> HandleOnGenerateRow(TSharedPtr<FPulldownRow> InItem, const TSharedRef<STableViewBase>& OwnerTable);

		// Called when an item in the list is clicked.
		void HandleOnMouseButtonClick(TSharedPtr<FPulldownRow> SelectedItem, const bool bIsSelectWhenDoubleClick);
		void HandleOnMouseButtonDoubleClick(TSharedPtr<FPulldownRow> SelectedItem, const bool bIsSelectWhenDoubleClick);
		void OnRowItemClicked(TSharedPtr<FPulldownRow> SelectedItem);

		// Returns whether the items in the list match the filter string.
		bool FilterRow(TSharedPtr<FPulldownRow> InItem) const;

		// Toggles whether the selected value is displayed even if it is displayed in items in the pull-down menu.
		void ToggleDisableDisplayText();

		// Returns whether the selected value is displayed even if it is displayed in items in the pull-down menu.
		static ECheckBoxState GetDisableDisplayTextCheckState();

		// Toggles whether to disable text coloring in the pull-down menu and display text for all items with the default color.
		void ToggleDisableTextColoring();

		// Returns whether to disable text coloring in the pull-down menu and display text for all items with the default color.
		static ECheckBoxState GetDisableTextColoringCheckState();
	
	private:
		// The search box widget for internal use.
		TSharedPtr<SSearchBox> SearchBox;
		
		// The list view widget for internal use.
		TSharedPtr<SListView<TSharedPtr<FPulldownRow>>> ListView;

		// The pointer to the underlying data to use when building ListItems.
		const TArray<TSharedPtr<FPulldownRow>>* ListItemsSource = nullptr;
	
		// The list of items to display in the list.
		TArray<TSharedPtr<FPulldownRow>> ListItems;

		// The list of shortcut keys used by this widget.
		TSharedPtr<FUICommandList> CommandBindings;
	
		// The string by which to filter language codes.
		FString FilterString;
	
		// The event that is called when a new item is selected.
		FOnSelectionChanged OnSelectionChanged;
	};
}
