// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "Templates/SubclassOf.h"
#include "TestPulldown.generated.h"

USTRUCT(BlueprintType)
struct FTestPulldown : public FPulldownStructBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 TestInt = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString TestString;
};

USTRUCT(BlueprintType)
struct FTestPulldown2 : public FPulldownStructBase
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FTestPulldown3 : public FPulldownStructBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UObject> AssetType;
};

USTRUCT(BlueprintType)
struct FTestPulldown4 : public FPulldownStructBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 NumOfRows = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName RowName;
};
