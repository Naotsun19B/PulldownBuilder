// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"
#include "PulldownBuilder/Types/PulldownRows.h"

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
		// Initializes pull-down menu and widget when the details panel is constructed.
		virtual void InitializePulldown();
		
		// Rebuilds the list of strings to display in the pull-down menu.
		virtual void RebuildPulldown();

		// Refreshes the widget in the pull-down menu.
		virtual void RefreshPulldownWidget();

		// Generates a list of strings to display in the pull-down menu.
		virtual FPulldownRows GenerateSelectableValues();

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

		// Applies default values to the pull-down structure being edited.
		void ApplyDefaultValue(const bool bForceApply = false);

		// Returns whether the currently edited pull-down struct has been changed at least once.
		bool IsEdited() const;
		
		// Gets or sets the value of a variable with the specified name.
		// If specify a property name that does not exist and get it, nullptr is returned.
		bool GetPropertyValue(const FName& PropertyName, const TFunction<void(const FString& PropertyValue)>& Predicate) const;
		bool GetPropertyValue(const FName& PropertyName, FString& StringPropertyValue) const;
		bool GetPropertyValue(const FName& PropertyName, FName& NamePropertyValue) const;
		bool GetPropertyValue(const FName& PropertyName, bool& bBoolPropertyValue) const;
		void SetPropertyValue(const FName& PropertyName, const FString& NewPropertyValue);
		void SetPropertyValue(const FName& PropertyName, const FName& NewNamePropertyValue);
		void SetPropertyValue(const FName& PropertyName, const bool bNewBoolPropertyValue);
		
	protected:
		// The list of values that can be set in FPulldownStructBase::SelectedValue.
		FPulldownRows SelectableValues;

		// The widget that displays a pull-down menu based on the SelectableValues.
		TSharedPtr<SPulldownSelectorComboButton> SelectedValueWidget;
	};
}
