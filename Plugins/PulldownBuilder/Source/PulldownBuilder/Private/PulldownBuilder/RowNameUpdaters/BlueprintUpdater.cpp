// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownBuilder/RowNameUpdaters/BlueprintUpdater.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "PulldownStruct/NativeLessPulldownStruct.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "Engine/Blueprint.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(BlueprintUpdater)
#endif

void UBlueprintUpdater::UpdateRowNamesInternal(
	UPulldownContents* PulldownContents,
	const FName& PreChangeSelectedValue,
	const FName& PostChangeSelectedValue
)
{
	EnumerateAssets<UBlueprint>([&](const UBlueprint* Blueprint) -> bool
	{
		return
			UpdateMemberVariables(Blueprint->GeneratedClass, PulldownContents, PreChangeSelectedValue, PostChangeSelectedValue) ||
			UpdateGraphPins(Blueprint, PulldownContents, PreChangeSelectedValue, PostChangeSelectedValue);
	});
}

bool UBlueprintUpdater::UpdateGraphPins(
	const UBlueprint* Blueprint,
	const UPulldownContents* PulldownContents,
	const FName& PreChangeSelectedValue,
	const FName& PostChangeSelectedValue
)
{
	bool bIsModified = false;
	
	TArray<UEdGraph*> Graphs;
	Blueprint->GetAllGraphs(Graphs);
	for (const auto& Graph : Graphs)
	{
		if (!IsValid(Graph))
		{
			continue;
		}

		TArray<UEdGraphNode*> Nodes = Graph->Nodes;
		for (const auto& Node : Nodes)
		{
			if (!IsValid(Node))
			{
				continue;
			}

			TArray<UEdGraphPin*> Pins = Node->Pins;
			for (const auto& Pin : Pins)
			{
				if (Pin == nullptr)
				{
					continue;
				}

				if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
				{
					if (const auto* Struct = Cast<UScriptStruct>(Pin->PinType.PinSubCategoryObject))
					{
						if (PulldownBuilder::FPulldownBuilderUtils::IsPulldownStruct(Struct) &&
							Struct == PulldownContents->GetPulldownStructType().SelectedStruct)
						{
							const TSharedPtr<FString> CurrentSelectedValue = PulldownBuilder::FPulldownBuilderUtils::StructStringToMemberValue(
								Pin->DefaultValue,
								GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue)
							);
							if (CurrentSelectedValue.IsValid() && **CurrentSelectedValue == PreChangeSelectedValue)
							{
								const TSharedPtr<FString> UpdatedSelectedValue = PulldownBuilder::FPulldownBuilderUtils::MemberValueToStructString(
									Pin->DefaultValue,
									GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue),
									PostChangeSelectedValue.ToString()
								);
								if (UpdatedSelectedValue.IsValid())
								{
									Pin->DefaultValue = *UpdatedSelectedValue;
									bIsModified = true;
								}
							}
						}
						else if (PulldownBuilder::FPulldownBuilderUtils::IsNativeLessPulldownStruct(Struct))
						{
							const TSharedPtr<FString> CurrentPulldownSource = PulldownBuilder::FPulldownBuilderUtils::StructStringToMemberValue(
								Pin->DefaultValue,
								GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource)
							);
							if (CurrentPulldownSource.IsValid() && **CurrentPulldownSource == PulldownContents->GetFName())
							{
								const TSharedPtr<FString> CurrentSelectedValue = PulldownBuilder::FPulldownBuilderUtils::StructStringToMemberValue(
									Pin->DefaultValue,
									GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, SelectedValue)
								);
								if (CurrentSelectedValue.IsValid() && **CurrentSelectedValue == PreChangeSelectedValue)
								{
									const TSharedPtr<FString> UpdatedSelectedValue = PulldownBuilder::FPulldownBuilderUtils::MemberValueToStructString(
										Pin->DefaultValue,
										GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, SelectedValue),
										PostChangeSelectedValue.ToString()
									);
									if (UpdatedSelectedValue.IsValid())
									{
										Pin->DefaultValue = *UpdatedSelectedValue;
										bIsModified = true;
									}
								}
							}
						}	
					}
				}
			}
		}
	}

	return bIsModified;
}
