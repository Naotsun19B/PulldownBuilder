// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "TestActor.h"

void ATestActor::TestFunction(FTestPulldown2 InTest)
{
	Test = InTest;
}

bool UTestActorIdentifierNameRegistry::SupportsActorClass_Implementation(const UClass* ActorClass) const
{
	return ActorClass->IsChildOf<ATestActor>();
}

FString UTestActorIdentifierNameRegistry::GetActorIdentifierName_Implementation(const AActor* Actor) const
{
	const auto* TestActor = CastChecked<ATestActor>(Actor);
	return FString::Printf(TEXT("%s_%s"), *GetNameSafe(ATestActor::StaticClass()), *LexToString(TestActor->Test));
}
