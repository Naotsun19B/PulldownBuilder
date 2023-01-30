// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "PreviewPulldownStruct.generated.h"

/**
* Structure for previewing pull-down menus in the PulldownContents.
*/
USTRUCT(NotBlueprintType, BlueprintInternalUseOnly)
struct FPreviewPulldownStruct : public FPulldownStructBase
{
	GENERATED_BODY()
};
