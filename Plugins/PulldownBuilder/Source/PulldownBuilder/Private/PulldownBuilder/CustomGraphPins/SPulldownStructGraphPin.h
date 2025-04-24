// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"

struct FPulldownRow;
struct FStructContainer;
class UPulldownContents;

namespace PulldownBuilder
{
	class SPulldownSelectorComboButton;
	
	/**
	 * A custom graph pin widget that applies to structures that inherit from FPulldownStructBase.
	 */
	class PULLDOWNBUILDER_API SPulldownStructGraphPin : public SGraphPin
	{
	public:
		SLATE_BEGIN_ARGS(SPulldownStructGraphPin) {}
		SLATE_END_ARGS()

		// Constructor.
		void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

		// Destructor.
		virtual ~SPulldownStructGraphPin() override;

		// SGraphPin interface.
		virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
		// End of SGraphPin interface.

	protected:
		// Rebuilds the list of strings to display in the pull-down menu.
		virtual void RebuildPulldown();

		// Refreshes the widget in the pull-down menu.
		virtual void RefreshPulldownWidget();

		// Generates a list of strings to display in the pull-down menu.
		virtual TArray<TSharedPtr<FPulldownRow>> GenerateSelectableValues();

		// Returns the PulldownContents asset associated with the currently edited pull-down struct.
		virtual UPulldownContents* GetRelatedPulldownContents() const;

		// Returns type information of the pull-down struct currently being edited.
		const UScriptStruct* GetRelatedPulldownStructType() const;
		
		// Generates a widget that displays a pull-down menu.
		TSharedRef<SWidget> GenerateSelectableValuesWidget();
	
		// Finds for the same name as the specified name from the SelectableValues.
		TSharedPtr<FPulldownRow> FindSelectableValueByName(const FName& InName) const;

		// Returns the selected item.
		TSharedPtr<FPulldownRow> GetSelection() const;

		// Returns the individual panel size set in PulldownContents.
		float GetIndividualPanelHeight() const;
		float GetIndividualPanelWidth() const;
		
		// Called when the value of the SelectedValueWidget changes.
		void OnSelectedValueChanged(TSharedPtr<FPulldownRow> SelectedItem, ESelectInfo::Type SelectInfo);

		// Updates the value of a pull-down struct's searchable object.
		void UpdateSearchableObject();
		
		// Gets or sets the value of a variable with the specified name.
		// If specify a property name that does not exist and get it, nullptr is returned.
		TSharedPtr<FName> GetPropertyValue(const FName& PropertyName) const;
		void SetPropertyValue(const FName& PropertyName, const FName& NewPropertyValue);
		
	protected:
		// The list of values that can be set in FPulldownStructBase::SelectedValue.
		TArray<TSharedPtr<FPulldownRow>> SelectableValues;

		// The widget that displays a pull-down menu based on the SelectableValues.
		TSharedPtr<SPulldownSelectorComboButton> SelectedValueWidget;
	};
}
