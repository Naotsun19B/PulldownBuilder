// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PulldownStruct/Utilities/ActorIdentifierNameRegistry.h"
#include "TestPulldown.h"
#include "TestActor.generated.h"

UCLASS(Blueprintable)
class PULLDOWNBUILDERTEST_API ATestActor : public AActor
{
	GENERATED_BODY()

public:
	// AActor interface.
	virtual void BeginPlay() override;
	// End of AActor interface.
	
	UFUNCTION(BlueprintCallable)
	void TestFunction(FTestPulldown2 InTest);

	void LogTest();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTestPulldown2 Test;
};

UCLASS(NotBlueprintable)
class UTestActorIdentifierNameRegistry : public UActorIdentifierNameRegistry
{
	GENERATED_BODY()

public:
	// UActorIdentifierNameRegistry interface.
	virtual bool SupportsActorClass_Implementation(const UClass* ActorClass) const override;
	virtual FString GetActorIdentifierName_Implementation(const AActor* Actor) const override;
	// End of UActorIdentifierNameRegistry interface.
};
