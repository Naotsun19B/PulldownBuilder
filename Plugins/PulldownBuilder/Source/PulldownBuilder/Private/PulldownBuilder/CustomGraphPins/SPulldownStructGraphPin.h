﻿// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"

class SPulldownSelectorComboButton;
struct FPulldownRow;

/**
 * A custom graph pin that applies to structures that inherit from FPulldownStructBase.
 */
class PULLDOWNBUILDER_API SPulldownStructGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SPulldownStructGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

	// SGraphPin interface.
	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
	// End of SGraphPin interface.

protected:
	// Rebuilds the list of strings to display in the pull-down menu.
	virtual void RebuildPulldown();

	// Refresh the widget in the pull-down menu.
	virtual void RefreshPulldownWidget();

	// Generates a list of strings to display in the pull-down menu.
	virtual TArray<TSharedPtr<FPulldownRow>> GenerateSelectableValues();

	// Generates a widget that displays a pull-down menu.
	TSharedRef<SWidget> GenerateSelectableValuesWidget();
	
	// Search for the same name as the specified name from the SelectableValues.
	// If not found, returns nullptr.
	TSharedPtr<FPulldownRow> FindSelectableValueByName(const FName& InName) const;
	
	// Called when the value of the SelectedValueWidget changes.
	void OnSelectedValueChanged(TSharedPtr<FPulldownRow> SelectedItem, ESelectInfo::Type SelectInfo);

	// Gets or sets the value of a variable with the specified name.
	// If specify a property name that does not exist and get it, nullptr is returned.
	TSharedPtr<FName> GetPropertyValue(const FName& PropertyName) const;
	void SetPropertyValue(const FName& PropertyName, const FName& NewPropertyValue);
	
protected:
	// A list of values that can be set in FPulldownStructBase::SelectedValue.
	TArray<TSharedPtr<FPulldownRow>> SelectableValues;

	// A widget that displays a pull-down menu based on the SelectableValues.
	TSharedPtr<SPulldownSelectorComboButton> SelectedValueWidget;
};
