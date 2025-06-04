// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Kismet/BlueprintFunctionLibrary.h"
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

/**
 * A utility class that defines the process of building SelectedValue for pulldown structures constructed from the default UActorNamePulldownListGenerator.
 */
UCLASS()
class PULLDOWNSTRUCT_API UDefaultActorNamePulldownProcess : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Returns the persistent level or sub level to which the specified actor belongs.
	UFUNCTION(BlueprintPure, Category = "Pulldown Builder|Actor Name")
	static UWorld* GetWorldToActorBelong(const AActor* Actor);
	
	// Returns the unique name of the world to which it belongs based on the specified actor.
	// If UActorNamePulldownProcessOverride does not override processing, it returns the asset name.
	UFUNCTION(BlueprintPure, Category = "Pulldown Builder|Actor Name")
	static FString GetWorldIdentifierName(const UWorld* World);
	
	// Returns the unique name of the actor in the process registered in the registry.
	// If there is no UActorIdentifierNameRegistry that the specified actor is supported, the result of AActor::GetName is returned.
	UFUNCTION(BlueprintPure, Category = "Pulldown Builder|Actor Name")
	static FString GetActorIdentifierName(const AActor* Actor);

	// Splits a built SelectedValue into a WorldIdentifierName and ActorIdentifierName.
	// Supports the format of "WorldIdentifierName::ActorIdentifierName" if it is not overridden in UActorNamePulldownProcessOverride.
	UFUNCTION(BlueprintCallable, Category = "Pulldown Builder|Actor Name")
	static bool SplitSelectedValueToWorldIdentifierNameAndActorIdentifierName(
		const FString& SelectedValue,
		FString& WorldIdentifierName,
		FString& ActorIdentifierName
	);

	// Builds a SelectedValue from the WorldIdentifierName and ActorIdentifierName.
	// If it is not overridden in UActorNamePulldownProcessOverride, constructs it in the format "WorldIdentifierName::ActorIdentifierName".
	UFUNCTION(BlueprintCallable, Category = "Pulldown Builder|Actor Name")
	static FString BuildSelectedValueFromWorldIdentifierNameAndActorIdentifierName(
		const FString& WorldIdentifierName,
		const FString& ActorIdentifierName
	);
};
