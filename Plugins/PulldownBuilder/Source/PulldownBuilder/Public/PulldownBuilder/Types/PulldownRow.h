// Copyright 2021-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownRow.generated.h"

/**
 * A struct that summarizes the data used in the pull-down menu row.
 */
USTRUCT(BlueprintType)
struct FPulldownRow
{
	GENERATED_BODY()

public:
	// The variable to store the item selected in the pull-down menu.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulldown")
	FString SelectedValue;
	
	// The text displayed in a pull-down menu row.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulldown")
	FText DisplayText;

	// The tooltip text that appears when you hover over a row in a pull-down menu.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulldown")
	FText TooltipText;

public:
	// Constructor.
	FPulldownRow()
		: SelectedValue(FName(NAME_None).ToString())
	{
	}
	explicit FPulldownRow(
		const FString& InSelectedValue,
		const FText& InTooltipText = FText::GetEmpty(),
		const FText& InDisplayText = FText::GetEmpty()
	)
		: SelectedValue(InSelectedValue)
		, DisplayText(InDisplayText)
		, TooltipText(InTooltipText)
	{
	}

	// Overload operators.
	FORCEINLINE bool operator ==(const FPulldownRow& Other) const
	{
		return SelectedValue.Equals(Other.SelectedValue);
	}

	FORCEINLINE bool operator !=(const FPulldownRow& Other) const
	{
		return !(*this == Other);
	}
	// End of overload operators.

	// Returns the text displayed in a pull-down menu row.
	FText GetDisplayText() const
	{
		return (DisplayText.IsEmpty() ? FText::FromString(SelectedValue) : DisplayText);
	}
};

// Define a GetTypeHash function so that it can be used as a map key.
FORCEINLINE uint32 GetTypeHash(const FPulldownRow& PulldownRow)
{
	return GetTypeHash(PulldownRow.SelectedValue);
}
