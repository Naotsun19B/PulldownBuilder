// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownStruct/Utilities/ActorIdentifierNameRegistry.h"

bool UActorIdentifierNameRegistry::SupportsActorClass_Implementation(const TSubclassOf<AActor>& ActorClass) const
{
	unimplemented();
	return false;
}

FString UActorIdentifierNameRegistry::GetActorIdentifierName_Implementation(const AActor* Actor) const
{
	unimplemented();
	return {};
}
