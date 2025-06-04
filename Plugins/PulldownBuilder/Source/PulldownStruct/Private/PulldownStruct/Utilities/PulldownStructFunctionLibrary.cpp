// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownStruct/Utilities/PulldownStructFunctionLibrary.h"
#include "PulldownStruct/Utilities/ActorNamePulldownProcessOverride.h"
#include "PulldownStruct/Utilities/ActorIdentifierNameRegistry.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "UObject/UObjectIterator.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(PulldownStructFunctionLibrary)
#endif

UActorNamePulldownProcessOverride* UPulldownStructFunctionLibrary::FindActorNamePulldownProcessOverride()
{
	for (auto* ActorNamePulldownProcessOverride : TObjectRange<UActorNamePulldownProcessOverride>(RF_NoFlags))
	{
		const UClass* ActorNamePulldownProcessOverrideClass = ActorNamePulldownProcessOverride->GetClass();
		verify(IsValid(ActorNamePulldownProcessOverrideClass));
		if (ActorNamePulldownProcessOverrideClass->HasAnyClassFlags(CLASS_Abstract))
		{
			continue;
		}

		return ActorNamePulldownProcessOverride;
	}

	return nullptr;
}

UWorld* UPulldownStructFunctionLibrary::GetWorldToActorBelong(const AActor* Actor)
{
	if (const UActorNamePulldownProcessOverride* OverriddenProcess = FindActorNamePulldownProcessOverride())
	{
		return OverriddenProcess->GetWorldToActorBelong(Actor);
	}
	
	return UDefaultActorNamePulldownProcess::GetWorldToActorBelong(Actor);
}

FString UPulldownStructFunctionLibrary::GetWorldIdentifierName(const UWorld* World)
{
	if (const UActorNamePulldownProcessOverride* OverriddenProcess = FindActorNamePulldownProcessOverride())
	{
		return OverriddenProcess->GetWorldIdentifierName(World);
	}
	
	return UDefaultActorNamePulldownProcess::GetWorldIdentifierName(World);
}

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

	return UDefaultActorNamePulldownProcess::GetActorIdentifierName(Actor);
}

bool UPulldownStructFunctionLibrary::SplitSelectedValueToWorldIdentifierNameAndActorIdentifierName(
	const FString& SelectedValue,
	FString& WorldIdentifierName,
	FString& ActorIdentifierName
)
{
	if (UActorNamePulldownProcessOverride* OverriddenProcess = FindActorNamePulldownProcessOverride())
	{
		return OverriddenProcess->SplitSelectedValueToWorldIdentifierNameAndActorIdentifierName(SelectedValue, WorldIdentifierName, ActorIdentifierName);
	}
	
	return UDefaultActorNamePulldownProcess::SplitSelectedValueToWorldIdentifierNameAndActorIdentifierName(SelectedValue, WorldIdentifierName, ActorIdentifierName);
}

FString UPulldownStructFunctionLibrary::BuildSelectedValueFromWorldIdentifierNameAndActorIdentifierName(
	const FString& WorldIdentifierName,
	const FString& ActorIdentifierName
)
{
	if (UActorNamePulldownProcessOverride* OverriddenProcess = FindActorNamePulldownProcessOverride())
	{
		return OverriddenProcess->BuildSelectedValueFromWorldIdentifierNameAndActorIdentifierName(WorldIdentifierName, ActorIdentifierName);
	}
	
	return UDefaultActorNamePulldownProcess::BuildSelectedValueFromWorldIdentifierNameAndActorIdentifierName(WorldIdentifierName, ActorIdentifierName);
}

AActor* UPulldownStructFunctionLibrary::FindActorByPulldownStruct(
	const UObject* WorldContextObject,
	const FPulldownStructBase& PulldownStruct,
	const TSubclassOf<AActor>& ActorClass /* = AActor::StaticClass() */,
	const FOnFilterActor& FilterPredicate /* = FOnFilterActor() */
)
{
	FString WorldIdentifierName;
	FString ActorIdentifierName;
	{
		const FString SelectedValue = LexToString(PulldownStruct);
		if (!SplitSelectedValueToWorldIdentifierNameAndActorIdentifierName(SelectedValue, WorldIdentifierName, ActorIdentifierName))
		{
			return nullptr;
		}
	}
	
	verify(IsValid(GEngine));
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	const FString PersistentLevelIdentifierName = GetWorldIdentifierName(World);
	if (PersistentLevelIdentifierName != WorldIdentifierName)
	{
		bool bWasBelongingLevelFound = false;
		const TArray<ULevel*>& Levels = World->GetLevels();
		for (const auto* Level : Levels)
		{
			if (!IsValid(Level))
			{
				continue;
			}

			const FString SubLevelIdentifierName = GetWorldIdentifierName(Level->GetTypedOuter<UWorld>());
			if (SubLevelIdentifierName == WorldIdentifierName)
			{
				bWasBelongingLevelFound = true;
				break;
			}
		}

		if (!bWasBelongingLevelFound)
		{
			return nullptr;
		}
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
