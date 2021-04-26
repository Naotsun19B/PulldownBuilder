// Copyright 2021 Naotsun. All Rights Reserved.

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

#if WITH_EDITORONLY_DATA
public:
	// The name of the PulldownContents asset from which the pull-down menu is based.
	UPROPERTY()
	FName PulldownContentsName;
#endif
};
