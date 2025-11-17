// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Package.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "PulldownStructType.generated.h"

/**
 * A struct for specifying the type of struct that inherits from FPulldownStructBase.
 */
USTRUCT(BlueprintInternalUseOnly)
struct FPulldownStructType
{
	GENERATED_BODY()

public:
	// A StaticStruct pointer for the specified struct.
	UPROPERTY(EditAnywhere, Category = "Pulldown")
	UScriptStruct* SelectedStruct;

public:
	// Constructor.
	FPulldownStructType() : SelectedStruct(nullptr) {}
	explicit FPulldownStructType(UScriptStruct* InStructType) : SelectedStruct(InStructType) {}
	explicit FPulldownStructType(const FName& InStructTypeName)
	{
		SelectedStruct = FindObject<UScriptStruct>(
#if UE_5_01_OR_LATER
			nullptr,
#else
			ANY_PACKAGE,
#endif
			*InStructTypeName.ToString(),
#if UE_5_07_OR_LATER
			EFindObjectFlags::ExactClass
#else
			true
#endif
		);
	}

	// Overload operators.
	FORCEINLINE bool operator ==(const FPulldownStructType& Other) const
	{
		return (SelectedStruct == Other.SelectedStruct);
	}

	FORCEINLINE bool operator !=(const FPulldownStructType& Other) const
	{
		return !(*this == Other);
	}
	// End of overload operators.
	
	// Returns whether the struct information is valid.
	bool IsValid() const { return ::IsValid(SelectedStruct); }
};

// Define a GetTypeHash function so that it can be used as a map key.
FORCEINLINE uint32 GetTypeHash(const FPulldownStructType& PulldownStructType)
{
	return GetTypeHash(PulldownStructType.SelectedStruct);
}
