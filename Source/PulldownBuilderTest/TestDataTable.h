// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TestDataTable.generated.h"

// USTRUCT(meta = (TooltipProperty = "TestString"))
// As mentioned above, you can use the meta specifier to change the properties displayed in the tooltip.
USTRUCT(meta = (TextColorProperty = "TestColor"))
struct FNativeDataTableStruct : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 TestInt = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString TestString;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FString PulldownTooltip;

	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	// FLinearColor PulldownTextColor;
	// You can also define a property named "PulldownTextColor" and set the text color as mentioned above.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FColor TestColor = FColor::White;
};
