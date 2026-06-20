// Copyright 2021-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ActorNamePulldownProcessOverride.generated.h"

/**
 * A class that overrides the process of building SelectedValue for pull-down structs constructed from UActorNamePulldownListGenerator.
 * Used to switch to functionality of this plugin if the user already implements a similar feature.
 *
 * Scope: a single subclass is picked via TObjectRange and applied **project-wide**.
 * For per-actor-class identifier customization (without changing the project-wide format), use
 * UActorIdentifierNameRegistry instead -- see that header for the dispatch order and when to pick which.
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
 * A utility class that exposes the *default* (non-overridden, non-registry) implementations used when
 * UPulldownStructFunctionLibrary resolves an actor-name pull-down value.
 *
 * Dispatch order in UPulldownStructFunctionLibrary:
 *   1. If a UActorNamePulldownProcessOverride subclass exists, its implementation is used.
 *   2. For GetActorIdentifierName only, any UActorIdentifierNameRegistry that supports the actor class is used next.
 *   3. Otherwise this default class is used.
 *
 * Public so that user code overriding UActorNamePulldownProcessOverride can chain back into the default
 * implementation. End-user / Blueprint code should normally call UPulldownStructFunctionLibrary instead,
 * which performs the routing above.
 */
UCLASS()
class PULLDOWNSTRUCT_API UDefaultActorNamePulldownProcess : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Default implementation: returns the persistent level or sub level to which the specified actor belongs.
	UFUNCTION(BlueprintPure, Category = "Pulldown Builder|Actor Name")
	static UWorld* GetWorldToActorBelong(const AActor* Actor);

	// Default implementation: returns FSoftObjectPath(World).GetAssetName() as the identifier.
	UFUNCTION(BlueprintPure, Category = "Pulldown Builder|Actor Name")
	static FString GetWorldIdentifierName(const UWorld* World);

	// Default implementation: returns GetNameSafe(Actor). Does NOT consult UActorIdentifierNameRegistry --
	// the registry lookup is performed by UPulldownStructFunctionLibrary::GetActorIdentifierName.
	UFUNCTION(BlueprintPure, Category = "Pulldown Builder|Actor Name")
	static FString GetActorIdentifierName(const AActor* Actor);

	// Default implementation: splits SelectedValue using PulldownBuilder::Global::WorldAndActorDelimiter ("::").
	UFUNCTION(BlueprintCallable, Category = "Pulldown Builder|Actor Name")
	static bool SplitSelectedValueToWorldIdentifierNameAndActorIdentifierName(
		const FString& SelectedValue,
		FString& WorldIdentifierName,
		FString& ActorIdentifierName
	);

	// Default implementation: builds a SelectedValue in the format "WorldIdentifierName::ActorIdentifierName".
	UFUNCTION(BlueprintCallable, Category = "Pulldown Builder|Actor Name")
	static FString BuildSelectedValueFromWorldIdentifierNameAndActorIdentifierName(
		const FString& WorldIdentifierName,
		const FString& ActorIdentifierName
	);
};
