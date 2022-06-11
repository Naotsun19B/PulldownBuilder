// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Package.h"
#include "PulldownStructType.generated.h"

/**
 * A structure for specifying the type of structure that inherits from FPulldownStructBase.
 */
USTRUCT(BlueprintInternalUseOnly)
struct FPulldownStructType
{
	GENERATED_BODY()

public:
	// StaticStruct pointer for the specified structure.
	UPROPERTY(EditAnywhere, Category = "Pulldown")
	UScriptStruct* SelectedStruct;

public:
	// Constructor.
	FPulldownStructType() : SelectedStruct(nullptr) {}
	explicit FPulldownStructType(UScriptStruct* InStructType) : SelectedStruct(InStructType) {}
	explicit FPulldownStructType(const FName& InStructTypeName)
	{
		SelectedStruct = FindObject<UScriptStruct>(ANY_PACKAGE, *InStructTypeName.ToString(), true);
	}

	// Overload oprators.
	FORCEINLINE bool operator ==(const FPulldownStructType& Other) const
	{
		return (SelectedStruct == Other.SelectedStruct);
	}

	FORCEINLINE bool operator !=(const FPulldownStructType& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE FName operator *() const
	{
		if (IsValid(SelectedStruct))
		{
			return SelectedStruct->GetFName();
		}

		return NAME_None;
	}
	// End of overload oprators.
	
	// Returns whether the structure information is empty.
	bool IsEmpty() const { return !IsValid(SelectedStruct); }
};

// Define a GetTypeHash function so that it can be used as a map key.
FORCEINLINE uint32 GetTypeHash(const FPulldownStructType& PulldownStructType)
{
	return GetTypeHash(PulldownStructType.SelectedStruct);
}
