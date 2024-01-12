// Copyright 2021-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "PreviewPulldownStruct.generated.h"

/**
* A struct that previewing pull-down menus in the PulldownContents.
*/
USTRUCT(NotBlueprintType, BlueprintInternalUseOnly)
struct FPreviewPulldownStruct : public FPulldownStructBase
{
	GENERATED_BODY()
};
