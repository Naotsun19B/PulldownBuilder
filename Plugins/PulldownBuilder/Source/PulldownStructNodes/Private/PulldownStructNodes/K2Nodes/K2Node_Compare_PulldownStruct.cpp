// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/K2Nodes/K2Node_Compare_PulldownStruct.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "FindInBlueprintManager.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintFieldNodeSpawner.h"
#include "KismetCompiler.h"
#include "K2Node_CallFunction.h"
#include "K2Node_BreakStruct.h"
#include "K2Node_TemporaryVariable.h"

#define LOCTEXT_NAMESPACE "K2Node_Compare_PulldownStruct"

const FName UK2Node_Compare_PulldownStruct::LhsPinName = TEXT("Lhs");
const FName UK2Node_Compare_PulldownStruct::RhsPinName = TEXT("Rhs");

FText UK2Node_Compare_PulldownStruct::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (IsValid(PulldownStruct))
	{
		if (CachedNodeTitle.IsOutOfDate(this))
		{
			CachedNodeTitle.SetCachedText(
				FText::Format(
					LOCTEXT("NodeTitleFormat", "{CompareMethodName} ({StructName})"),
					GetCompareMethodName(),
					PulldownStruct->GetDisplayNameText()
				),
				this
			);
		}
		
		return CachedNodeTitle;
	}
	
	return LOCTEXT("NodeTitle", "Equal (Unknown Pulldown Struct)");
}

FText UK2Node_Compare_PulldownStruct::GetKeywords() const
{
	return GetCompareMethodOperator();
}

void UK2Node_Compare_PulldownStruct::AddPinSearchMetaDataInfo(const UEdGraphPin* Pin, TArray<FSearchTagDataPair>& OutTaggedMetaData) const
{
	Super::AddPinSearchMetaDataInfo(Pin, OutTaggedMetaData);

	if (!IsValid(PulldownStruct) && Pin != nullptr)
	{
		const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
		check(!IsValid(K2Schema));

		if (K2Schema->IsExecPin(*Pin) && Pin->Direction == EGPD_Output && PulldownStruct->IsNative())
		{
			// Allow native struct pins to be searchable by C++ struct name.
			OutTaggedMetaData.Add(
				FSearchTagDataPair(
					FFindInBlueprintSearchTags::FiB_NativeName,
					FText::FromString(Pin->GetName())
				)
			);
		}
	}
}

void UK2Node_Compare_PulldownStruct::AllocateDefaultPins()
{
	if (IsValid(PulldownStruct))
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, PulldownStruct, LhsPinName);
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, PulldownStruct, RhsPinName);
	}
	else
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, LhsPinName);
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, RhsPinName);
	}
	
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, UEdGraphSchema_K2::PN_ReturnValue);
}

bool UK2Node_Compare_PulldownStruct::IsNodePure() const
{
	return true;
}

bool UK2Node_Compare_PulldownStruct::ShouldDrawCompact() const
{
	return true;
}

FText UK2Node_Compare_PulldownStruct::GetCompactNodeTitle() const
{
	return GetCompareMethodOperator();
}

FText UK2Node_Compare_PulldownStruct::GetMenuCategory() const
{
	return FText::Format(
		LOCTEXT("MenuCategory", "Pulldown Struct|{CompareMethodName}"),
		GetCompareMethodName()
	);
}

void UK2Node_Compare_PulldownStruct::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	ActionRegistrar.RegisterStructActions(
		FBlueprintActionDatabaseRegistrar::FMakeStructSpawnerDelegate::CreateWeakLambda(
			this, [this](const UScriptStruct* Struct) -> UBlueprintNodeSpawner*
			{
				if (!PulldownBuilder::FPulldownBuilderUtils::IsPulldownStruct(Struct))
				{
					return nullptr;
				}
				
				UBlueprintFieldNodeSpawner* NodeSpawner = UBlueprintFieldNodeSpawner::Create(GetClass(), Struct);
				if (!IsValid(NodeSpawner))
				{
					return nullptr;
				}

				struct FNodeFieldSetter
				{
				public:
					static void SetNodeStruct(UEdGraphNode* NewNode, FFieldVariant Field)
					{
						auto* Struct = Field.Get<UScriptStruct>();
						if (!IsValid(Struct))
						{
							return;
						}
			
						if (auto* CastedNode = Cast<UK2Node_Compare_PulldownStruct>(NewNode))
						{
							CastedNode->PulldownStruct = Struct;
						}
					}
				};
	
				NodeSpawner->SetNodeFieldDelegate = UBlueprintFieldNodeSpawner::FSetNodeFieldDelegate::CreateStatic(
					&FNodeFieldSetter::SetNodeStruct
				);

				return NodeSpawner;
			}
		)
	);
}

void UK2Node_Compare_PulldownStruct::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	if (IsValid(PulldownStruct))
	{
		auto* CompareNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
		check(IsValid(CompareNode));
		CompareNode->FunctionReference.SetExternalMember(
			GetFunctionName(),
			UKismetMathLibrary::StaticClass()
		);
		CompareNode->AllocateDefaultPins();

		auto LinkSourcePinToCompareNodePin = [&](const FName& SourcePinName, const FName& CompareNodePinName)
		{
			const UEdGraphSchema_K2* K2Schema = CompilerContext.GetSchema();
			check(IsValid(K2Schema));

			auto* BreakNode = CompilerContext.SpawnIntermediateNode<UK2Node_BreakStruct>(this, SourceGraph);
			check(IsValid(BreakNode));
			BreakNode->StructType = PulldownStruct;
			BreakNode->bMadeAfterOverridePinRemoval = true;
			BreakNode->AllocateDefaultPins();
			
			UEdGraphPin* SourcePin = FindPinChecked(SourcePinName, EGPD_Input);
			UEdGraphPin* BreakNodePin = BreakNode->FindPinChecked(PulldownStruct->GetFName(), EGPD_Input);
			check(SourcePin != nullptr && BreakNodePin != nullptr);
			// #TODO: I have to deal with the case where the pin is the default value.
			// if (SourcePin->LinkedTo.Num() == 0)
			// {
			// 	
			// }
			// else
			{
				CompilerContext.MovePinLinksToIntermediate(*SourcePin, *BreakNodePin);
			}
			
			UEdGraphPin* BreakNodeValuePin = BreakNode->FindPinChecked(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue), EGPD_Output);
			UEdGraphPin* CompareNodePin = CompareNode->FindPinChecked(CompareNodePinName, EGPD_Input);
			check(BreakNodeValuePin != nullptr && CompareNodePin != nullptr)
			K2Schema->TryCreateConnection(BreakNodeValuePin, CompareNodePin);
		};

		LinkSourcePinToCompareNodePin(LhsPinName, TEXT("A"));
		LinkSourcePinToCompareNodePin(RhsPinName, TEXT("B"));

		{
			UEdGraphPin* SourceReturnValuePin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue, EGPD_Output);
			UEdGraphPin* CompareNodeReturnValuePin = CompareNode->GetReturnValuePin();
			check(SourceReturnValuePin != nullptr && CompareNodeReturnValuePin != nullptr);
			CompilerContext.MovePinLinksToIntermediate(*SourceReturnValuePin, *CompareNodeReturnValuePin);
		}
	}
	
	BreakAllNodeLinks();
}

void UK2Node_Compare_PulldownStruct::PreloadRequiredAssets()
{
	if (IsValid(PulldownStruct))
	{
		PreloadObject(PulldownStruct);
	}
}

#undef LOCTEXT_NAMESPACE
