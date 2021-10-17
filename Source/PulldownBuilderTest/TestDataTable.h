// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TestPulldown.h"
#include "PulldownStruct/NativeLessPulldownStruct.h"
#include "TestDataTable.generated.h"

USTRUCT()
struct FNativeDataTableStruct : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 TestInt;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString TestString;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FString PulldownTooltip;
};

USTRUCT()
struct FTestDataTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FTestPulldown Test;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FTestPulldown2 Test1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FNativeLessPulldownStruct NativeLess;
};
