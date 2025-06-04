// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownStruct/Utilities/ActorNamePulldownProcessOverride.h"

UWorld* UActorNamePulldownProcessOverride::GetWorldToActorBelong_Implementation(const AActor* Actor) const
{
	unimplemented();
	return nullptr;
}

FString UActorNamePulldownProcessOverride::GetWorldIdentifierName_Implementation(const UWorld* World) const
{
	unimplemented();
	return {};
}

bool UActorNamePulldownProcessOverride::SplitSelectedValueToWorldIdentifierNameAndActorIdentifierName_Implementation(
	const FString& SelectedValue,
	FString& WorldIdentifierName,
	FString& ActorIdentifierName
)
{
	unimplemented();
	return false;
}

FString UActorNamePulldownProcessOverride::BuildSelectedValueFromWorldIdentifierNameAndActorIdentifierName_Implementation(
	const FString& WorldIdentifierName,
	const FString& ActorIdentifierName
)
{
	unimplemented();
	return {};
}
