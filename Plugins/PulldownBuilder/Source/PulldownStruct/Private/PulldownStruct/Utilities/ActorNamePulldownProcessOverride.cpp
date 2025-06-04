// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownStruct/Utilities/ActorNamePulldownProcessOverride.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "UObject/SoftObjectPath.h"

UWorld* UActorNamePulldownProcessOverride::GetWorldToActorBelong_Implementation(const AActor* Actor) const
{
	return UDefaultActorNamePulldownProcess::GetWorldToActorBelong(Actor);
}

FString UActorNamePulldownProcessOverride::GetWorldIdentifierName_Implementation(const UWorld* World) const
{
	return UDefaultActorNamePulldownProcess::GetWorldIdentifierName(World);
}

bool UActorNamePulldownProcessOverride::SplitSelectedValueToWorldIdentifierNameAndActorIdentifierName_Implementation(
	const FString& SelectedValue,
	FString& WorldIdentifierName,
	FString& ActorIdentifierName
)
{
	return UDefaultActorNamePulldownProcess::SplitSelectedValueToWorldIdentifierNameAndActorIdentifierName(SelectedValue, WorldIdentifierName, ActorIdentifierName);
}

FString UActorNamePulldownProcessOverride::BuildSelectedValueFromWorldIdentifierNameAndActorIdentifierName_Implementation(
	const FString& WorldIdentifierName,
	const FString& ActorIdentifierName
)
{
	return UDefaultActorNamePulldownProcess::BuildSelectedValueFromWorldIdentifierNameAndActorIdentifierName(WorldIdentifierName, ActorIdentifierName);
}

UWorld* UDefaultActorNamePulldownProcess::GetWorldToActorBelong(const AActor* Actor)
{
	if (const auto* LevelToBelongTo = Actor->GetTypedOuter<ULevel>())
	{
		return Cast<UWorld>(LevelToBelongTo->GetOuter());
	}

	return nullptr;
}

FString UDefaultActorNamePulldownProcess::GetWorldIdentifierName(const UWorld* World)
{
	return FSoftObjectPath(World).GetAssetName();
}

FString UDefaultActorNamePulldownProcess::GetActorIdentifierName(const AActor* Actor)
{
	return GetNameSafe(Actor);
}

bool UDefaultActorNamePulldownProcess::SplitSelectedValueToWorldIdentifierNameAndActorIdentifierName(
	const FString& SelectedValue,
	FString& WorldIdentifierName,
	FString& ActorIdentifierName
)
{
	return SelectedValue.Split(PulldownBuilder::Global::WorldAndActorDelimiter, &WorldIdentifierName, &ActorIdentifierName);
}

FString UDefaultActorNamePulldownProcess::BuildSelectedValueFromWorldIdentifierNameAndActorIdentifierName(
	const FString& WorldIdentifierName,
	const FString& ActorIdentifierName
)
{
	return (WorldIdentifierName + PulldownBuilder::Global::WorldAndActorDelimiter + ActorIdentifierName);
}
