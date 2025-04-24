// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "NativeLessPulldownStruct.generated.h"

/**
 * A pull-down struct that can be defined without using C++.
 * Toggle the pull-down menu by specifying PulldownContents.
 */
USTRUCT(BlueprintType)
struct PULLDOWNSTRUCT_API FNativeLessPulldownStruct : public FPulldownStructBase
{
	GENERATED_BODY()
	SETUP_PULLDOWN_STRUCT()

public:
	// The name of the PulldownContents asset from which the pull-down menu is based.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulldown")
	FName PulldownSource;

public:
	// Constructor.
	FNativeLessPulldownStruct();
	FNativeLessPulldownStruct(const FName& InPulldownSource, const FName& InSelectedValue);
};
SETUP_PULLDOWN_STRUCT_OPS(FNativeLessPulldownStruct)

// Specializes some of the function templates, such as operators defined in FPulldownStructBase.
template<>
FORCEINLINE_DEBUGGABLE bool operator==(const FNativeLessPulldownStruct& Lhs, const FNativeLessPulldownStruct& Rhs)
{
	return (
		Lhs.SelectedValue.IsEqual(Rhs.SelectedValue, ENameCase::CaseSensitive) &&
		Lhs.PulldownSource.IsEqual(Rhs.PulldownSource, ENameCase::CaseSensitive)
	);
}

template<>
FORCEINLINE_DEBUGGABLE uint32 GetTypeHash(const FNativeLessPulldownStruct& PulldownStruct)
{
	uint32 Hash = 0;
	
	Hash = HashCombine(Hash, GetTypeHash(PulldownStruct.SelectedValue));
	Hash = HashCombine(Hash, GetTypeHash(PulldownStruct.PulldownSource));
	
	return Hash;
}
