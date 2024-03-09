// Copyright 2021-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "PulldownStruct/PulldownStructMacros.h"
#include "NativeLessPulldownStruct.generated.h"

/**
 * A pull-down struct that can be defined without using C++.
 * Toggle the pull-down menu by specifying PulldownContents.
 */
USTRUCT(BlueprintType)
struct FNativeLessPulldownStruct : public FPulldownStructBase
{
	GENERATED_BODY()
	SETUP_PULLDOWN_STRUCT()

public:
	// The name of the PulldownContents asset from which the pull-down menu is based.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulldown")
	FName PulldownSource;

public:
	// Constructor.
	FNativeLessPulldownStruct() : FPulldownStructBase(NAME_None), PulldownSource(NAME_None) {}
	FNativeLessPulldownStruct(const FName& InPulldownSource, const FName& InSelectedValue)
		: FPulldownStructBase(InPulldownSource)
		, PulldownSource(InSelectedValue)
	{
	}
};
SETUP_PULLDOWN_STRUCT_OPS(FNativeLessPulldownStruct)
