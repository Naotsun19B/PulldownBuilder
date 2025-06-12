// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/ActorNamePulldownListGenerator.h"
#include "PulldownStruct/Utilities/PulldownStructFunctionLibrary.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "GameFramework/Actor.h"
#include "UObject/UObjectThreadContext.h"
#include "Editor.h"
#if UE_5_00_OR_LATER
#include "Subsystems/UnrealEditorSubsystem.h"
#endif
#include "EngineUtils.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(ActorNamePulldownListGenerator)
#endif

UActorNamePulldownListGenerator::UActorNamePulldownListGenerator()
	: ActorClass(AActor::StaticClass())
	, bIncludeInheritedActorClasses(true)
{
}

FPulldownRows UActorNamePulldownListGenerator::GetPulldownRows(
	const TArray<UObject*>& OuterObjects,
	const FStructContainer& StructInstance
) const
{
	auto* EditorWorld = []() -> UWorld*
	{
		check(IsValid(GEditor));
#if UE_5_00_OR_LATER
		auto* UnrealEditorSubsystem = GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>();
		check(IsValid(UnrealEditorSubsystem));
		return UnrealEditorSubsystem->GetEditorWorld();
#else
		return GEditor->GetEditorWorldContext(true).World();
#endif
	}();
	if (!IsValid(EditorWorld))
	{
		return FPulldownRows::Empty;
	}
	
	FPulldownRows PulldownRows;
	
	for (const auto* Actor : TActorRange<AActor>(EditorWorld, ActorClass))
	{
		if (!IsValid(Actor))
		{
			continue;
		}

		if (!FilterActor(Actor))
		{
			continue;
		}

		const auto* LevelToBelongTo = Actor->GetTypedOuter<ULevel>();
		if (!IsValid(LevelToBelongTo))
		{
			continue;
		}

		const UWorld* World = UPulldownStructFunctionLibrary::GetWorldToActorBelong(Actor);
		if (!IsValid(World))
		{
			continue;
		}
		
		const FString WorldIdentifierName = UPulldownStructFunctionLibrary::GetWorldIdentifierName(World);
		const FString ActorIdentifierName = UPulldownStructFunctionLibrary::GetActorIdentifierName(Actor);
			
		FPulldownRow PulldownRow;
		PulldownRow.SelectedValue = UPulldownStructFunctionLibrary::BuildSelectedValueFromWorldIdentifierNameAndActorIdentifierName(WorldIdentifierName, ActorIdentifierName);
		PulldownRow.TooltipText = FText::FromString(Actor->GetPathName());
		PulldownRow.DisplayText = FText::FromString(Actor->GetActorLabel());
			
		PulldownRows.Add(PulldownRow);
	}
	
	return PulldownRows;
}

bool UActorNamePulldownListGenerator::SupportsSwitchNode_Implementation() const
{
	return false;
}

bool UActorNamePulldownListGenerator::IsEnableCustomDefaultValue_Implementation() const
{
	// The default value is not supported because the actor may not exist.
	return true;
}

bool UActorNamePulldownListGenerator::FilterActor_Implementation(const AActor* TestActor) const
{
	check(IsValid(TestActor));

	const UClass* TestActorClass = TestActor->GetClass();
	check(IsValid(TestActorClass));
	
	if (bIncludeInheritedActorClasses)
	{
		if (ActorClassesToExclude.Contains(TestActor->GetClass()))
		{
			return false;
		}
	}
	else if (TestActorClass->GetClass() != ActorClass)
	{
		return false;
	}
	
	for (const auto& RequiredInterface : RequiredInterfaces)
	{
		if (!TestActorClass->ImplementsInterface(RequiredInterface))
		{
			return false;
		}
	}
	
	return true;
}

UClass* UActorNamePulldownListGenerator::GetActorClass() const
{
	return ActorClass;
}
