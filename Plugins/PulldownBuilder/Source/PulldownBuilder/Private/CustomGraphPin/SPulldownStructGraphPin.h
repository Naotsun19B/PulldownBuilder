// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"

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

private:
	// Rebuilds the list of strings to display in the pull-down menu.
	void RebuildPulldown();

	// Search for the same name as the specified Name from the list of
	// character strings displayed in the pull-down menu.
	// If not found, returns nullptr.
	TSharedPtr<FString> FindDisplayStringByName(const FName& InName) const;
	
	// Called when the value of the combo box changes.
	void OnStateValueChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo);

	// Getters and setters for FPulldownStructBase::SelectedValue for structures to
	// which this custom graph pin is applied.
	FName GetSelectedValueData() const;
	void SetSelectedValueData(const FName& NewSelectedValue);
	
private:
	// A list of strings to display in the pull-down menu.
	TArray<TSharedPtr<FString>> DisplayStrings;

	// The currently selected value.
	FName SelectedValue;

	// Pull-down menu widget.
	TSharedPtr<STextComboBox> PulldownWidget;
};
