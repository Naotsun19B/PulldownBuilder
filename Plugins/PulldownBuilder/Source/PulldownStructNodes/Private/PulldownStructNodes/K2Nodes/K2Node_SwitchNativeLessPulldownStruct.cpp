﻿// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/K2Nodes/K2Node_SwitchNativeLessPulldownStruct.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Types/StructContainer.h"
#include "PulldownBuilder/Types/PulldownRow.h"
#include "PulldownStruct/NativeLessPulldownStruct.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_SwitchNativeLessPulldownStruct)
#endif

#define LOCTEXT_NAMESPACE "K2Node_SwitchNativeLessPulldownStruct"

UK2Node_SwitchNativeLessPulldownStruct::UK2Node_SwitchNativeLessPulldownStruct()
{
	bHasDefaultPin = true;
	PulldownStruct = FNativeLessPulldownStruct::StaticStruct();
}

void UK2Node_SwitchNativeLessPulldownStruct::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UK2Node_SwitchNativeLessPulldownStruct, PulldownContents))
	{
		ReconstructNode();
	}
}

void UK2Node_SwitchNativeLessPulldownStruct::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	
	if (Ar.IsSaving() && Ar.IsLoading())
	{
		Ar << PulldownContents;
	}
}

FText UK2Node_SwitchNativeLessPulldownStruct::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	const FText& OriginalNodeTitle = Super::GetNodeTitle(TitleType);
	if (PulldownContents.IsNull())
	{
		return OriginalNodeTitle;
	}

	return FText::Format(
		LOCTEXT("NodeTitleFormat", "{0} ({1})"),
		OriginalNodeTitle,
		FText::FromString(PulldownContents.GetAssetName())
	);
}

void UK2Node_SwitchNativeLessPulldownStruct::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(ActionKey);
		check(IsValid(NodeSpawner));
		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

bool UK2Node_SwitchNativeLessPulldownStruct::ShouldShowNodeProperties() const
{
	return true;
}

void UK2Node_SwitchNativeLessPulldownStruct::FillSelectedValues()
{
	if (!PulldownContents.IsValid())
	{
		return;
	}
	
	const UEdGraphPin* SelectionPin = GetSelectionPin();
	if (SelectionPin == nullptr)
	{
		return;
	}

	FStructContainer StructContainer;
	if (!PulldownBuilder::FPulldownBuilderUtils::GenerateStructContainerFromPin(SelectionPin, StructContainer))
	{
		return;
	}

	const TArray<TSharedPtr<FPulldownRow>>& PulldownRows = PulldownContents->GetPulldownRows(
		TArray<UObject*>{ PulldownBuilder::FPulldownBuilderUtils::GetOuterAssetFromPin(SelectionPin) },
		StructContainer
	);
	
	// If the timing is too early and the PulldownContents have not been loaded, the serialized value will be used as is.
	if (PulldownRows.Num() == 0)
	{
		return;
	}

	SelectedValues.Reset(PulldownRows.Num());

	for (const auto& PulldownRow : PulldownRows)
	{
		if (!PulldownRow.IsValid())
		{
			continue;
		}

		SelectedValues.Add(*PulldownRow->DisplayText.ToString());
	}
}

#undef LOCTEXT_NAMESPACE
