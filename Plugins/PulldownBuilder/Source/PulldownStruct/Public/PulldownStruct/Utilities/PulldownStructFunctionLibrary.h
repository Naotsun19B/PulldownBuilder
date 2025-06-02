// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "PulldownStructFunctionLibrary.generated.h"

/**
 * A utility class that defines the functions to be used at the runtime of a pull-down struct.
 */
UCLASS()
class PULLDOWNSTRUCT_API UPulldownStructFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Returns the unique name of the actor in the process registered in the registry.
	// If there is no UActorIdentifierNameRegistry that the specified actor is supported, the result of AActor::GetName is returned.
	UFUNCTION(Blueprintable, Category = "Pulldown Builder")
	static FString GetActorIdentifierName(const AActor* Actor);
	
	// The event that already implements a similar function and when switching to this plugin,
	// it will work even from the SelectedValue format, which is already implemented.
	DECLARE_DELEGATE_RetVal_ThreeParams(
		bool, FOnSplitSelectedValueToWorldNameAndActorIdentifierName,
		const FString& /* SelectedValue */,
		FString& /* WorldName */,
		FString& /* ActorIdentifierName */
	);
	static FOnSplitSelectedValueToWorldNameAndActorIdentifierName OnSplitSelectedValueToWorldNameAndActorIdentifierName;
	
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
