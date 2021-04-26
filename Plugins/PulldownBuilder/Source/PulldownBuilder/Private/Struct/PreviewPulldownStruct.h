// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownStructBase.h"
#include "PreviewPulldownStruct.generated.h"

/**
* Structure for previewing pulldown menus in the PulldownContents.
*/
USTRUCT(NotBlueprintType, BlueprintInternalUseOnly)
struct FPreviewPulldownStruct : public FPulldownStructBase
{
	GENERATED_BODY()
};
