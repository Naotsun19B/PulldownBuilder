﻿// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/K2Nodes/K2Node_Compare_PulldownStruct.h"
#include "PulldownStructNodes/Utilities/PulldownStructNodeUtils.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "Kismet/KismetMathLibrary.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintFieldNodeSpawner.h"
#include "KismetCompiler.h"
#include "K2Node_CallFunction.h"

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
					LOCTEXT("NodeTitleFormat", "{0} ({1})"),
					GetCompareMethodName(),
					PulldownStruct->GetDisplayNameText()
				),
				this
			);
		}
		
		return CachedNodeTitle;
	}
	
	return FText::Format(
		LOCTEXT("NodeTitle", "{0} (Unknown Pulldown Struct)"),
		GetCompareMethodName()
	);
}

FText UK2Node_Compare_PulldownStruct::GetKeywords() const
{
	return GetCompareMethodOperator();
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
		LOCTEXT("MenuCategoryFormat", "Utilities|Operators|Pulldown Struct|{0}"),
		GetCompareMethodName()
	);
}

void UK2Node_Compare_PulldownStruct::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	ActionRegistrar.RegisterStructActions(
		FBlueprintActionDatabaseRegistrar::FMakeStructSpawnerDelegate::CreateUObject(
			this, &UK2Node_Compare_PulldownStruct::HandleOnMakeStructSpawner
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
			
			UEdGraphPin* SourcePin = FindPinChecked(SourcePinName, EGPD_Input);
			UEdGraphPin* CompareNodePin = CompareNode->FindPinChecked(CompareNodePinName, EGPD_Input);

			check(
				PulldownBuilder::FPulldownStructNodeUtils::LinkPulldownStructPinToNamePin(
					this,
					CompilerContext,
					K2Schema,
					SourcePin,
					CompareNodePin
				)
			);
		};

		LinkSourcePinToCompareNodePin(LhsPinName, TEXT("A"));
		LinkSourcePinToCompareNodePin(RhsPinName, TEXT("B"));

		{
			UEdGraphPin* SourceReturnValuePin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue, EGPD_Output);
			UEdGraphPin* CompareNodeReturnValuePin = CompareNode->GetReturnValuePin();
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

UBlueprintNodeSpawner* UK2Node_Compare_PulldownStruct::HandleOnMakeStructSpawner(const UScriptStruct* Struct) const
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
		static void SetNodeField(UEdGraphNode* NewNode, FFieldVariant Field)
		{
			if (auto* Struct = Field.Get<UScriptStruct>())
			{
				if (auto* CastedNode = Cast<UK2Node_Compare_PulldownStruct>(NewNode))
				{
					CastedNode->PulldownStruct = Struct;
				}
			}
		}
	};
	
	NodeSpawner->SetNodeFieldDelegate = UBlueprintFieldNodeSpawner::FSetNodeFieldDelegate::CreateStatic(
		&FNodeFieldSetter::SetNodeField
	);

	return NodeSpawner;
}

#undef LOCTEXT_NAMESPACE
