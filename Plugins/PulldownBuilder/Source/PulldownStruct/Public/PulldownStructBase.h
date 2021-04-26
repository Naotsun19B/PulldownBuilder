// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownStructBase.generated.h"

/**
 * Base structure of the structure displayed as a pull-down menu.
 * To create a pull-down menu structure in C++, define a structure that inherits this structure.
 * If the inherited child struct is used only internally and cannot be used from within Blueprint and
 * cannot be selected by FPulldownStructType, it is necessary to specify "NotBlueprintType" and
 * "BlueprintInternalUseOnly" in the USTRUCT of the child structure.
 */
USTRUCT(BlueprintInternalUseOnly)
struct FPulldownStructBase
{
	GENERATED_BODY()

public:
	// Variable to store the item selected in the pull-down menu.
	// Stores the value used at runtime.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SelectedValue;

public:
	// Constructor.
	FPulldownStructBase() : SelectedValue(NAME_None) {}
	FPulldownStructBase(const FName& InValue) : SelectedValue(InValue) {}

	// Overload oprators.
	FORCEINLINE bool operator ==(const FName& Other) const
	{
		return (SelectedValue == Other);
	}

	FORCEINLINE bool operator !=(const FName& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE const FName& operator *() const
	{
		return SelectedValue;
	}
	// End of overload oprators.

	// Define a GetTypeHash function so that it can be used as a map key.
	friend FORCEINLINE uint32 GetTypeHash(const FPulldownStructBase& PulldownStruct)
	{
		return GetTypeHash(PulldownStruct.SelectedValue);
	}
};
