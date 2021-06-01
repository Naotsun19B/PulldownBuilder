// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TestPulldown.h"
#include "NativeLessPulldownStruct.h"
#include "TestDataAsset.generated.h"

UCLASS()
class PULLDOWNBUILDERTEST_API UTestDataAsset : public UDataAsset
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
