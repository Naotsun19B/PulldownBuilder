// Copyright 2021-2023 Naotsun. All Rights Reserved.

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
	// A text displayed in a pull-down menu row.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulldown")
	FText DisplayText;

	// A tooltip text that appears when you hover over a row in a pull-down menu.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulldown")
	FText TooltipText;

public:
	// Constructor.
	FPulldownRow() : DisplayText(FText::FromName(NAME_None)) {}
	FPulldownRow(const FText& InDisplayText, const FText& InTooltipText)
		: DisplayText(InDisplayText)
		, TooltipText(InTooltipText)
	{
	}
	explicit FPulldownRow(const FText& InDisplayText)
		: DisplayText(InDisplayText)
	{
	}
	FPulldownRow(const FString& InDisplayString, const FString& InTooltipString)
		: DisplayText(FText::FromString(InDisplayString))
		, TooltipText(FText::FromString(InTooltipString))
	{
	}
	explicit FPulldownRow(const FString& InDisplayString)
		: DisplayText(FText::FromString(InDisplayString))
	{
	}
	FPulldownRow(const FName& InDisplayName, const FName& InTooltipName)
		: DisplayText(FText::FromName(InDisplayName))
		, TooltipText(FText::FromName(InTooltipName))
	{
	}
	explicit FPulldownRow(const FName& InDisplayName)
		: DisplayText(FText::FromName(InDisplayName))
	{
	}

	// Overload oprators.
	FORCEINLINE bool operator ==(const FPulldownRow& Other) const
	{
		return (
			DisplayText.EqualTo(Other.DisplayText) &&
			TooltipText.EqualTo(Other.TooltipText)
		);
	}

	FORCEINLINE bool operator !=(const FPulldownRow& Other) const
	{
		return !(*this == Other);
	}
	// End of overload oprators.
	
	// Define a GetTypeHash function so that it can be used as a map key.
	friend FORCEINLINE uint32 GetTypeHash(const FPulldownRow& PulldownRow)
	{
		return GetTypeHash(PulldownRow.DisplayText.ToString());
	}
};
