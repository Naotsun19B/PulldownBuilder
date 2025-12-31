// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownBuilderTest/TestActor.h"
#include "PulldownBuilderTest/PulldownBuilderTestFunctionLibrary.h"

void ATestActor::BeginPlay()
{
	Super::BeginPlay();

	LogTest();
}

void ATestActor::TestFunction(FTestPulldown2 InTest)
{
	Test = InTest;
	LogTest();
}

void ATestActor::LogTest()
{
	static int32 LogCounter = 0;
	UPulldownBuilderTestFunctionLibrary::Log(
		FText::Format(
			NSLOCTEXT("TestActor", "LogTestFormat", "Test Actor Property Test({0})\n{1} : Test = {2}"),
			LogCounter++,
			FText::FromString(GetName()),
			Test.ToText()
		)
	);
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
