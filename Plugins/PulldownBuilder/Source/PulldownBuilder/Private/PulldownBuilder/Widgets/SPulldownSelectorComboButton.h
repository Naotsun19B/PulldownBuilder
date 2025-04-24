// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SComboButton.h"
#include "PulldownBuilder/Widgets/SPulldownSelector.h"

struct FPulldownRow;

namespace PulldownBuilder
{
	class SPulldownSelector;
	
	/**
	 * A widget that displays the PulldownSelector when you press a button and
	 * allows you to select an item from the pull-down menu.
	 */
	class PULLDOWNBUILDER_API SPulldownSelectorComboButton : public SComboButton
	{
	public:
		// Define the event to get the selected item.
		DECLARE_DELEGATE_RetVal(TSharedPtr<FPulldownRow>, FGetSelection);
		
	public:
		SLATE_BEGIN_ARGS(SPulldownSelectorComboButton)
			: _ListItemsSource(nullptr)
		{}

		// A source data that is the basis of the items displayed in the pull-down menu.
		SLATE_ARGUMENT(const TArray<TSharedPtr<FPulldownRow>>*, ListItemsSource)

		// Called when getting the selected item.
		SLATE_EVENT(FGetSelection, GetSelection)

		// Sets the vertical size of this widget.
		// If not set, the one selected in the editor settings will be used.
		SLATE_ATTRIBUTE(float, HeightOverride)

		// Sets the horizontal size of this widget.
		// If not set, the one selected in the editor settings will be used.
		SLATE_ATTRIBUTE(float, WidthOverride)

		// Whether you need to double-click to select an item.
		// If not set, the one selected in the editor settings will be used.
		SLATE_ATTRIBUTE(bool, bIsSelectWhenDoubleClick)
		
		// Called when a new item is selected.
		SLATE_EVENT(SPulldownSelector::FOnSelectionChanged, OnSelectionChanged)

		// Called when the button is pressed and the PulldownSelector is displayed.
		SLATE_EVENT(FOnComboBoxOpened, OnComboBoxOpened)
	
		SLATE_END_ARGS()

		// Constructor.
		void Construct(const FArguments& InArgs);

		// SWidget interface.
		virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
		// End of SWidget interface.
		
		// Getters and setters for the selected item.
		TSharedPtr<FPulldownRow> GetSelectedItem() const;
		void SetSelectedItem(TSharedPtr<FPulldownRow> NewItem);
	
		// Rebuilds the list.
		void RefreshList();

	private:
		// Returns the widget to display when the combo button is pressed.
		TSharedRef<SWidget> HandleOnGetMenuContent();
	
		// Returns the text displayed on the combo button.
		FText GetDisplayText() const;

		// Returns the text of the tooltip that is displayed when the combo button is on-cursor.
		FText GetTooltipText() const;
		
		// Called when the value of the PulldownSelector changes.
		void HandleOnSelectionChanged(TSharedPtr<FPulldownRow> SelectedItem, ESelectInfo::Type SelectInfo);
	
	private:
		// The pull-down selector widget to display in the contents of the combo box.
		TSharedPtr<SPulldownSelector> PulldownSelector;
		
		// The pointer to the underlying data to use when building ListItems.
		const TArray<TSharedPtr<FPulldownRow>>* ListItemsSource = nullptr;
	
		// The source data for the currently selected PulldownRow.
		TSharedPtr<FPulldownRow> SelectedPulldownRow;

		// The event to get selected item.
		FGetSelection GetSelection;

		// Sets the vertical size of this widget.
		TAttribute<float> HeightOverride;

		// Sets the horizontal size of this widget.
		TAttribute<float> WidthOverride;

		// Whether you need to double-click to select an item.
		TAttribute<bool> IsSelectWhenDoubleClick;
		
		// The event that is called when the value of the PulldownSelector changes.
		SPulldownSelector::FOnSelectionChanged OnSelectionChanged;
	};
}
