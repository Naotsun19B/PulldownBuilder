// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "Templates/SubclassOf.h"
#include "TestPulldown.generated.h"

USTRUCT(BlueprintType)
struct FTestPulldown : public FPulldownStructBase
{
	GENERATED_BODY()
	SETUP_PULLDOWN_STRUCT()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 TestInt = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString TestString;
};
SETUP_PULLDOWN_STRUCT_OPS(FTestPulldown)

USTRUCT(BlueprintType)
struct FTestPulldown2 : public FPulldownStructBase
{
	GENERATED_BODY()
	SETUP_PULLDOWN_STRUCT()
};
SETUP_PULLDOWN_STRUCT_OPS(FTestPulldown2)

USTRUCT(BlueprintType)
struct FTestPulldown3 : public FPulldownStructBase
{
	GENERATED_BODY()
	SETUP_PULLDOWN_STRUCT()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UObject> AssetType;
};
SETUP_PULLDOWN_STRUCT_OPS(FTestPulldown3)

USTRUCT(BlueprintType)
struct FTestPulldown4 : public FPulldownStructBase
{
	GENERATED_BODY()
	SETUP_PULLDOWN_STRUCT()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 NumOfRows = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName RowName;
};
SETUP_PULLDOWN_STRUCT_OPS(FTestPulldown4)
