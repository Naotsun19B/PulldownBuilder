// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestPulldown.h"
#include "TestActor.generated.h"

UCLASS(Blueprintable)
class PULLDOWNBUILDERTEST_API ATestActor : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "InTest"))
	void TestFunction(const FTestPulldown2& InTest)
	{
		Test = InTest;
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTestPulldown2 Test;
};
