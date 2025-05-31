// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ActorIdentifierNameRegistry.generated.h"

/**
 * A base class in the registry for implementing the unique name of actors used in
 * pull-down struct constructed from UActorNamePulldownListGenerator for each actor class.
 */
UCLASS(Abstract, Blueprintable)
class PULLDOWNSTRUCT_API UActorIdentifierNameRegistry : public UObject
{
	GENERATED_BODY()

public:
	// Returns whether the specified actor's class is supported by this registry.
	UFUNCTION(BlueprintNativeEvent, Category = "Actor Identifier Name Registry")
	bool SupportsActorClass(const TSubclassOf<AActor>& ActorClass) const;
	
	// Returns a unique name from the actor.
	UFUNCTION(BlueprintNativeEvent, Category = "Actor Identifier Name Registry")
	FString GetActorIdentifierName(const AActor* Actor) const;
};
