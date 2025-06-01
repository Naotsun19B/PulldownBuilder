// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownStruct/Utilities/ActorIdentifierNameRegistry.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(ActorIdentifierNameRegistry)
#endif

bool UActorIdentifierNameRegistry::SupportsActorClass_Implementation(const UClass* ActorClass) const
{
	unimplemented();
	return false;
}

FString UActorIdentifierNameRegistry::GetActorIdentifierName_Implementation(const AActor* Actor) const
{
	unimplemented();
	return {};
}
