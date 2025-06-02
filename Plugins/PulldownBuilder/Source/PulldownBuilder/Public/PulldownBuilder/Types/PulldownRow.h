// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownRow.generated.h"

struct FPulldownRow;

/**
 * A struct that summarizes the data used in the pull-down menu row.
 */
USTRUCT(BlueprintType)
struct PULLDOWNBUILDER_API FPulldownRow
{
	GENERATED_BODY()

public:
	// The variable to store the item selected in the pull-down menu.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulldown")
	FString SelectedValue;
	
	// The text displayed in a pull-down menu menu.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulldown")
	FText DisplayText;

	// The tooltip text that appears when you hover over a row in a pull-down menu.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulldown")
	FText TooltipText;

	// Whether this value is the default value for the configured pull-down struct.
	UPROPERTY(BlueprintReadWrite, Category = "Pulldown")
	bool bIsDefaultValue;

	// The color that is applied to the text displayed in a pull-down menu.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulldown")
	FLinearColor DisplayTextColor;

private:
	// Whether a value does not exist among the values displayed in the list in the pull-down menu.
	bool bIsNonExistentValue;

	// Non-existent values can only be created from FPulldownRows::SetNonExistentValue.
	friend struct FPulldownRows;

	// Defines the string for the None item used in SelectedValue.
	static const FString NoneString;

public:
	// Constructor.
	FPulldownRow();
	explicit FPulldownRow(
		const FString& InSelectedValue,
		const FText& InTooltipText = FText::GetEmpty(),
		const FText& InDisplayText = FText::GetEmpty()
	);

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

	// Returns whether no valid value is selected.
	bool IsNone() const;

	// Returns the text displayed in a pull-down menu row.
	FText GetDisplayText() const;
	
	// Returns whether a value does not exist among the values displayed in the list in the pull-down menu.
	bool IsNonExistentValue() const;
};

// Define a GetTypeHash function so that it can be used as a map key.
FORCEINLINE uint32 GetTypeHash(const FPulldownRow& PulldownRow)
{
	return GetTypeHash(PulldownRow.SelectedValue);
}
