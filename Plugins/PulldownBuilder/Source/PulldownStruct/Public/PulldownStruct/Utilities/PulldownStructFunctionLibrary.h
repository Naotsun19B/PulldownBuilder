// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "PulldownStructFunctionLibrary.generated.h"

class UActorNamePulldownProcessOverride;

/**
 * A utility class that defines the functions to be used at the runtime of a pull-down struct.
 */
UCLASS()
class PULLDOWNSTRUCT_API UPulldownStructFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Returns overridden processes that constructs a SelectedValue for a pulldown structure constructed from a UActorNamePulldownListGenerator.
	static UActorNamePulldownProcessOverride* FindActorNamePulldownProcessOverride();

	// Returns the world to which it belongs based on the specified actor.
	// If UActorNamePulldownProcessOverride does not override processing, it returns the world the actor belongs to.
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
	
	// Defines the delegate to use when filtering an actor.
	DECLARE_DELEGATE_RetVal_OneParam(bool, FOnFilterActor, const AActor& /* TestActor */);
	
	// Returns the actor found from the pull-down struct constructed by the UActorNamePulldownListGenerator.
	static AActor* FindActorByPulldownStruct(
		const UObject* WorldContextObject,
		const FPulldownStructBase& PulldownStruct,
		const TSubclassOf<AActor>& ActorClass = AActor::StaticClass(),
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
