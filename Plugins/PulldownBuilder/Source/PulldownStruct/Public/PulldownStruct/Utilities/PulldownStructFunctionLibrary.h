// Copyright 2021-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "PulldownStructFunctionLibrary.generated.h"

class UActorNamePulldownProcessOverride;

/**
 * A utility class that defines the functions to be used at the runtime of a pull-down struct.
 *
 * The Actor Name * functions below are the **public entry points** for runtime / Blueprint code.
 * Each performs the dispatch documented on UDefaultActorNamePulldownProcess
 * (UActorNamePulldownProcessOverride first, optionally UActorIdentifierNameRegistry next,
 * UDefaultActorNamePulldownProcess as the final fallback). Prefer these over calling the
 * default class directly.
 */
UCLASS()
class PULLDOWNSTRUCT_API UPulldownStructFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Returns overridden processes that constructs a SelectedValue for a pulldown structure constructed from a UActorNamePulldownListGenerator.
	static UActorNamePulldownProcessOverride* FindActorNamePulldownProcessOverride();

	// Public entry point: returns the world the actor belongs to, routed through any UActorNamePulldownProcessOverride.
	// Falls back to UDefaultActorNamePulldownProcess::GetWorldToActorBelong when no override is registered.
	UFUNCTION(BlueprintPure, Category = "Pulldown Builder|Actor Name")
	static UWorld* GetWorldToActorBelong(const AActor* Actor);

	// Public entry point: returns the world's identifier name, routed through any UActorNamePulldownProcessOverride.
	// Falls back to UDefaultActorNamePulldownProcess::GetWorldIdentifierName when no override is registered.
	UFUNCTION(BlueprintPure, Category = "Pulldown Builder|Actor Name")
	static FString GetWorldIdentifierName(const UWorld* World);

	// Public entry point: returns the actor's identifier name.
	// First a UActorIdentifierNameRegistry that SupportsActorClass(...) is consulted; if none matches,
	// UDefaultActorNamePulldownProcess::GetActorIdentifierName (i.e. GetNameSafe(Actor)) is returned.
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
	
	// Defines the delegate to use when filtering an actor.
	DECLARE_DELEGATE_RetVal_OneParam(bool, FOnFilterActor, const AActor& /* TestActor */);
	
	// Returns the actor found from the pull-down struct constructed by the UActorNamePulldownListGenerator.
	static AActor* FindActorByPulldownStruct(
		const UObject* WorldContextObject,
		const FPulldownStructBase& PulldownStruct,
		const FOnFilterActor& FilterPredicate = FOnFilterActor()
	);

	// Returns the actor found from the pull-down struct constructed by the UActorNamePulldownListGenerator.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true, WorldContext = "WorldContextObject", AutoCreateRefTerm = "WorldAndActorIdentifierName, ActorClass"))
	static AActor* K2_FindActorByPulldownStruct(
		const UObject* WorldContextObject,
		const FName& WorldAndActorIdentifierName,
		const TSubclassOf<AActor>& ActorClass
	);
};
