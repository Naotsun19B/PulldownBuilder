// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownStruct/Utilities/PulldownStructFunctionLibrary.h"
#include "PulldownStruct/Utilities/ActorIdentifierNameRegistry.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "EngineUtils.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(PulldownStructFunctionLibrary)
#endif

FString UPulldownStructFunctionLibrary::GetActorIdentifierName(const AActor* Actor)
{
	for (const auto* ActorIdentifierNameRegistry : TObjectRange<UActorIdentifierNameRegistry>(RF_NoFlags))
	{
		const UClass* ActorIdentifierNameRegistryClass = ActorIdentifierNameRegistry->GetClass();
		verify(IsValid(ActorIdentifierNameRegistryClass));
		if (ActorIdentifierNameRegistryClass->HasAnyClassFlags(CLASS_Abstract))
		{
			continue;
		}

		UClass* ActorClass = Actor->GetClass();
		verify(IsValid(ActorClass));
		if (!ActorIdentifierNameRegistry->SupportsActorClass(ActorClass))
		{
			continue;
		}

		return ActorIdentifierNameRegistry->GetActorIdentifierName(Actor);
	}

	return GetNameSafe(Actor);
}

AActor* UPulldownStructFunctionLibrary::FindActorByPulldownStruct(
	const UObject* WorldContextObject,
	const FPulldownStructBase& PulldownStruct,
	const TSubclassOf<AActor>& ActorClass /* = AActor::StaticClass() */,
	const FOnFilterActor& FilterPredicate /* = FOnFilterActor() */
)
{
	FString WorldName;
	FString ActorIdentifierName;
	{
		const FString SelectedValue = LexToString(PulldownStruct);
		if (!SelectedValue.Split(PulldownBuilder::Global::WorldAndActorDelimiter, &WorldName, &ActorIdentifierName))
		{
			return nullptr;
		}
	}
	
	verify(IsValid(GEngine));
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (WorldName != FSoftObjectPath(World).GetAssetName())
	{
		return nullptr;
	}
	
	for (auto* TestActor : TActorRange<AActor>(World, ActorClass))
	{
		if (!IsValid(TestActor))
		{
			continue;
		}

		if (FilterPredicate.IsBound() && !FilterPredicate.Execute(*TestActor))
		{
			continue;
		}

		const FString TestActorIdentifierName = GetActorIdentifierName(TestActor);
		if (ActorIdentifierName == TestActorIdentifierName)
		{
			return TestActor;
		}
	}
	
	return nullptr;
}

AActor* UPulldownStructFunctionLibrary::K2_FindActorByPulldownStruct(
	const UObject* WorldContextObject,
	const FName& WorldAndActorIdentifierName,
	const TSubclassOf<AActor>& ActorClass
)
{
	return FindActorByPulldownStruct(
		WorldContextObject,
		FPulldownStructBase(WorldAndActorIdentifierName),
		ActorClass
	);
}
