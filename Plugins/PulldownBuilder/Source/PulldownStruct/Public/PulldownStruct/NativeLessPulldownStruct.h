// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownStructBase.h"
#include "NativeLessPulldownStruct.generated.h"

/**
 * A pull-down structure that can be defined without using C++.
 * Toggle the pull-down menu by specifying PulldownContents.
 */
USTRUCT(BlueprintType)
struct FNativeLessPulldownStruct : public FPulldownStructBase
{
	GENERATED_BODY()

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
