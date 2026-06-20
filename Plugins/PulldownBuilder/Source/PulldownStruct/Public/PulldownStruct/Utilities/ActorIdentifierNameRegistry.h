// Copyright 2021-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Templates/SubclassOf.h"
#include "ActorIdentifierNameRegistry.generated.h"

/**
 * A base class in the registry for implementing the unique name of actors used in pull-down struct constructed
 * from UActorNamePulldownListGenerator for each actor class.
 *
 * Relationship with UActorNamePulldownProcessOverride
 * --------------------------------------------------
 * Both classes can customize how a pull-down value is built from an actor, but they operate at different scopes:
 *
 *   - UActorNamePulldownProcessOverride  : a **single, project-wide override** for the full SelectedValue pipeline
 *                                          (GetWorldToActorBelong / GetWorldIdentifierName /
 *                                           Split / Build SelectedValue). One subclass is selected via
 *                                          TObjectRange and used for every actor in the project.
 *
 *   - UActorIdentifierNameRegistry       : a **per-actor-class plugin** that only customizes the "identifier name"
 *                                          portion. Multiple registries can coexist; each declares which actor
 *                                          classes it supports via SupportsActorClass and is dispatched when its
 *                                          class matches.
 *
 * Dispatch order inside UPulldownStructFunctionLibrary::GetActorIdentifierName:
 *   1. UActorNamePulldownProcessOverride is *not* consulted here -- the override handles the SelectedValue split/build,
 *      not the per-actor identifier itself.
 *   2. Each UActorIdentifierNameRegistry is iterated; the first one whose SupportsActorClass returns true is used.
 *   3. UDefaultActorNamePulldownProcess::GetActorIdentifierName (GetNameSafe(Actor)) is the final fallback.
 *
 * When to pick which
 *   - Use **UActorNamePulldownProcessOverride** when you want one project-wide convention for how
 *     World / Actor identifiers are formatted, split, and joined back together (e.g. you want a different
 *     delimiter, or you want to encode sub-level information).
 *   - Use **UActorIdentifierNameRegistry** when most of the project should keep the default behaviour but
 *     a *specific actor class* (e.g. ATriggerBox, your custom AActor subclass) needs a custom identifier
 *     scheme. Several registries can be combined.
 */
UCLASS(Abstract, Blueprintable)
class PULLDOWNSTRUCT_API UActorIdentifierNameRegistry : public UObject
{
	GENERATED_BODY()

public:
	// Returns whether the specified actor's class is supported by this registry.
	// Implementations should return true only for the actor class hierarchy this registry is intended to handle;
	// returning true for AActor catches every actor and effectively shadows other registries.
	UFUNCTION(BlueprintNativeEvent, Category = "Actor Identifier Name")
	bool SupportsActorClass(const UClass* ActorClass) const;

	// Returns a unique name from the actor.
	// Only called for actors whose class passed SupportsActorClass; the returned name must be stable
	// (same actor instance must produce the same name across editor sessions) for pull-down lookups to work.
	UFUNCTION(BlueprintNativeEvent, Category = "Actor Identifier Name")
	FString GetActorIdentifierName(const AActor* Actor) const;
};
