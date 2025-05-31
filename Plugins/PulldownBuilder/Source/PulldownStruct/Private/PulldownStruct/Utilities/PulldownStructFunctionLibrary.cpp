// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownStruct/Utilities/PulldownStructFunctionLibrary.h"
#include "PulldownStruct/Utilities/ActorIdentifierNameRegistry.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "EngineUtils.h"

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

AActor* UPulldownStructFunctionLibrary::FindActorByPulldownStruct(
	const UObject* WorldContextObject,
	const int32& PulldownStruct,
	const TSubclassOf<AActor>& ActorClass
)
{
	unimplemented();
	return nullptr;
}

AActor* UPulldownStructFunctionLibrary::GenericFindActorByPulldownStruct(
	const UObject* WorldContextObject,
	const UScriptStruct* PulldownStructType,
	const void* PulldownStructRawData,
	const TSubclassOf<AActor>& ActorClass
)
{
	TOptional<FName> SelectedValue;
#if UE_4_25_OR_LATER
	for (const auto* NameProperty : TFieldRange<FNameProperty>(PulldownStructType))
#else
	for (const auto* NameProperty : TFieldRange<UNameProperty>(PulldownStructType))
#endif
	{
		if (NameProperty == nullptr)
		{
			continue;
		}

		if (NameProperty->GetFName() != GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue))
		{
			continue;
		}

		if (const FName* SelectedValuePtr = NameProperty->GetPropertyValuePtr_InContainer(PulldownStructRawData))
		{
			SelectedValue = *SelectedValuePtr;
			break;
		}
	}
	if (!SelectedValue.IsSet())
	{
		return nullptr;
	}

	const FPulldownStructBase PulldownStruct(SelectedValue.GetValue());
	return FindActorByPulldownStruct(WorldContextObject, PulldownStruct, ActorClass);
}

DEFINE_FUNCTION(UPulldownStructFunctionLibrary::execFindActorByPulldownStruct)
{
	P_GET_OBJECT(const UObject, WorldContextObject);
	P_GET_STRUCT_REF(FPulldownStructBase, PulldownStruct);
	const UScriptStruct* PulldownStructType = nullptr;
#if BEFORE_UE_4_24
	if (const auto* LhsProperty = Cast<UStructProperty>(Stack.MostRecentProperty))
#else
	if (const auto* LhsProperty = CastField<FStructProperty>(Stack.MostRecentProperty))
#endif
	{
		PulldownStructType = LhsProperty->Struct;
	}
	const void* PulldownStructRawData = &PulldownStructTemp;
	P_GET_OBJECT(UClass, ActorClass);
	
	P_FINISH;
	AActor* ReturnValue = nullptr;
		
	P_NATIVE_BEGIN;
	if (ensure(IsValid(PulldownStructType) && (PulldownStructRawData != nullptr)))
	{
		ReturnValue = GenericFindActorByPulldownStruct(WorldContextObject, PulldownStructType, PulldownStructRawData, ActorClass);
	}
	P_NATIVE_END;

	*static_cast<AActor**>(RESULT_PARAM) = ReturnValue;
}
