﻿// Copyright 2021-2022 Naotsun. All Rights Reserved.

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
		// Definition of an event to get the selected item.
		DECLARE_DELEGATE_RetVal(TSharedPtr<FPulldownRow>, FGetSelection);
		
	public:
		SLATE_BEGIN_ARGS(SPulldownSelectorComboButton)
			: _ListItemsSource(nullptr)
		{}

		// Data that is the basis of the items displayed in the pull-down menu.
		SLATE_ARGUMENT(const TArray<TSharedPtr<FPulldownRow>>*, ListItemsSource)

		// Called when getting the selected item.
		SLATE_EVENT(FGetSelection, GetSelection)

		// Called when a new item is selected.
		SLATE_EVENT(SPulldownSelector::FOnSelectionChanged, OnSelectionChanged)

		// Called when the button is pressed and the PulldownSelector is displayed.
		SLATE_EVENT(FOnComboBoxOpened, OnComboBoxOpened)
	
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs);

		// SWidget interface.
		virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
		// End of SWidget interface.
		
		// Getters and setters for the selected item.
		TSharedPtr<FPulldownRow> GetSelectedItem() const;
		void SetSelectedItem(TSharedPtr<FPulldownRow> NewItem);
	
		// Rebuild the list.
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
		// A pull-down selector widget to display in the contents of the combo box.
		TSharedPtr<SPulldownSelector> PulldownSelector;
		
		// A pointer to the underlying data to use when building ListItems.
		const TArray<TSharedPtr<FPulldownRow>>* ListItemsSource = nullptr;
	
		// Data for the currently selected PulldownRow.
		TSharedPtr<FPulldownRow> SelectedPulldownRow;

		// The event to get selected item.
		FGetSelection GetSelection;
		
		// The event that is called when the value of the PulldownSelector changes.
		SPulldownSelector::FOnSelectionChanged OnSelectionChanged;
	};
}
