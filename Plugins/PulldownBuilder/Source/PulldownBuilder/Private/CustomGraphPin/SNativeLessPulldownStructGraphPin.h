// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomGraphPin/SPulldownStructGraphPin.h"

class SSearchableTextComboBox;

/**
 * A custom graph pin that applies to structures that inherit from FNativeLessPulldownStruct.
 */
class PULLDOWNBUILDER_API SNativeLessPulldownStructGraphPin : public SPulldownStructGraphPin
{
public:
	SLATE_BEGIN_ARGS(SNativeLessPulldownStructGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

	// SGraphPin interface.
	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
	// End of SGraphPin interface.

private:
	// SPulldownStructGraphPin interface.
	virtual void RefreshPulldownWidget() override;
	virtual TArray<TSharedPtr<FString>> GenerateSelectableValues() override;
	// End of SPulldownStructGraphPin interface.
	
	// Search for the same name as the specified name from the PulldownContentsNames.
	// If not found, returns nullptr.
	TSharedPtr<FString> FindPulldownContentsNameByName(const FName& InName) const;
	
	// Called when the value of the PulldownSourceWidget changes.
	void OnPulldownSourceChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo);
	
private:
	// A list of values that can be set in FNativeLessPulldownStruct::PulldownSource.
	TArray<TSharedPtr<FString>> PulldownContentsNames;

	// A widget that displays a pull-down menu based on the PulldownContentsNames.
	TSharedPtr<SSearchableTextComboBox> PulldownSourceWidget;
};
