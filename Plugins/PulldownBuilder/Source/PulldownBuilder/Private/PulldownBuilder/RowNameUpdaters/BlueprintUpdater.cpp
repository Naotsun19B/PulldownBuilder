// Copyright 2021 Naotsun. All Rights Reserved.

#include "PulldownBuilder/RowNameUpdaters/BlueprintUpdater.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "PulldownStruct/NativeLessPulldownStruct.h"
#include "Engine/Blueprint.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"

void UBlueprintUpdater::UpdateRowNamesInternal(
	UPulldownContents* PulldownContents,
	const FName& PreChangeName,
	const FName& PostChangeName
)
{
	EnumerateAssets<UBlueprint>([&](UBlueprint* Blueprint) -> bool
	{
		return
			UpdateMemberVariables(Blueprint->GeneratedClass, PulldownContents, PreChangeName, PostChangeName) ||
			UpdateGraphPins(Blueprint, PulldownContents, PreChangeName, PostChangeName);
	});
}

bool UBlueprintUpdater::UpdateGraphPins(
	UBlueprint* Blueprint,
	UPulldownContents* PulldownContents,
	const FName& PreChangeName,
	const FName& PostChangeName
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
					if (auto* Struct = Cast<UScriptStruct>(Pin->PinType.PinSubCategoryObject))
					{
						if (FPulldownBuilderUtils::IsPulldownStruct(Struct) &&
							Struct == PulldownContents->GetPulldownStructType().SelectedStruct)
						{
							const TSharedPtr<FName> CurrentValue = FPulldownBuilderUtils::StructStringToMemberValue(
								Pin->DefaultValue,
								GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue)
							);
							if (CurrentValue.IsValid() && *CurrentValue == PreChangeName)
							{
								const TSharedPtr<FString> UpdatedValue = FPulldownBuilderUtils::MemberValueToStructString(
									Pin->DefaultValue,
									GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue),
									PostChangeName
								);
								if (UpdatedValue.IsValid())
								{
									Pin->DefaultValue = *UpdatedValue;
									bIsModified = true;
								}
							}
						}
						else if (FPulldownBuilderUtils::IsNativeLessPulldownStruct(Struct))
						{
							const TSharedPtr<FName> CurrentSource = FPulldownBuilderUtils::StructStringToMemberValue(
								Pin->DefaultValue,
								GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource)
							);
							if (CurrentSource.IsValid() && *CurrentSource == PulldownContents->GetFName())
							{
								const TSharedPtr<FName> CurrentValue = FPulldownBuilderUtils::StructStringToMemberValue(
									Pin->DefaultValue,
									GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, SelectedValue)
								);
								if (CurrentValue.IsValid() && *CurrentValue == PreChangeName)
								{
									const TSharedPtr<FString> UpdatedValue = FPulldownBuilderUtils::MemberValueToStructString(
										Pin->DefaultValue,
										GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, SelectedValue),
										PostChangeName
									);
									if (UpdatedValue.IsValid())
									{
										Pin->DefaultValue = *UpdatedValue;
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
