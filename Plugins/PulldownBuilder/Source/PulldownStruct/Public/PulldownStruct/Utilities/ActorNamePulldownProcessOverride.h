// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ActorNamePulldownProcessOverride.generated.h"

/**
 * A class that overrides the process of building SelectedValue for pull-down structs constructed from UActorNamePulldownListGenerator.
 * Used to switch to functionality of this plugin if the user already implements a similar feature.
 */
UCLASS(Abstract, Blueprintable)
class PULLDOWNSTRUCT_API UActorNamePulldownProcessOverride : public UObject
{
	GENERATED_BODY()

public:
	// Returns the world to which it belongs based on the specified actor.
	UFUNCTION(BlueprintNativeEvent, Category = "World Identifier Name")
	UWorld* GetWorldToActorBelong(const AActor* Actor) const;

	// Returns the unique name of the world to which it belongs based on the specified world.
	UFUNCTION(BlueprintNativeEvent, Category = "World Identifier Name")
	FString GetWorldIdentifierName(const UWorld* World) const;

	// Splits a built SelectedValue into a WorldIdentifierName and ActorIdentifierName.
	UFUNCTION(BlueprintNativeEvent, Category = "Selected Value")
	bool SplitSelectedValueToWorldIdentifierNameAndActorIdentifierName(
		const FString& SelectedValue,
		FString& WorldIdentifierName,
		FString& ActorIdentifierName
	);

	// Builds a SelectedValue from the WorldIdentifierName and ActorIdentifierName.
	UFUNCTION(BlueprintNativeEvent, Category = "Selected Value")
	FString BuildSelectedValueFromWorldIdentifierNameAndActorIdentifierName(
		const FString& WorldIdentifierName,
		const FString& ActorIdentifierName
	);
};
