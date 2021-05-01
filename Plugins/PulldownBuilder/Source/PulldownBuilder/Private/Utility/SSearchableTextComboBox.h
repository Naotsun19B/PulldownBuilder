// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SSearchableComboBox.h"

/**
 * A custom text combo box class with search capabilities.
 */
class PULLDOWNBUILDER_API SSearchableTextComboBox : public SSearchableComboBox
{
public:
	SLATE_BEGIN_ARGS(SSearchableTextComboBox)
		: _OptionsSource(nullptr)
		, _InitiallySelectedItem(nullptr)
	{}
	SLATE_ARGUMENT(const TArray<TSharedPtr<FString>>*, OptionsSource)
	SLATE_ARGUMENT(TSharedPtr<FString>, InitiallySelectedItem)
	SLATE_EVENT(FOnSelectionChanged, OnSelectionChanged)
	SLATE_EVENT(FOnComboBoxOpening, OnComboBoxOpening)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	
private:
	// Generates a widget for each row of the combo box.
	TSharedRef<SWidget> OnGenerateWidget(TSharedPtr<FString> InDisplayString);
};
