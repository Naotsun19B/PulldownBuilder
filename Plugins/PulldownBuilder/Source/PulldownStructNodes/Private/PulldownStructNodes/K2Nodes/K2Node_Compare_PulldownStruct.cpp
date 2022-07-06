// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/K2Nodes/K2Node_Compare_PulldownStruct.h"
#include "PulldownStruct/Utilities/PulldownStructFunctionLibrary.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "EdGraphSchema_K2.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "K2Node_Compare_PulldownStruct"

const FName UK2Node_Compare_PulldownStruct::LhsPinName = TEXT("Lhs");
const FName UK2Node_Compare_PulldownStruct::RhsPinName = TEXT("Rhs");

FText UK2Node_Compare_PulldownStruct::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType != ENodeTitleType::MenuTitle)
	{
		if (const UScriptStruct* ValueStruct = GetArgumentStructType())
		{
			if (CachedNodeTitle.IsOutOfDate(this))
			{
				CachedNodeTitle.SetCachedText(
					FText::Format(
						LOCTEXT("NodeTitleFormat", "{CompareMethodName} ({StructName})"),
						GetCompareMethodName(),
						ValueStruct->GetDisplayNameText()
					),
					this
				);
			}
		
			return CachedNodeTitle;
		}
	}
	
	return FText::Format(
		LOCTEXT("NodeTitle", "{CompareMethodName} (Pulldown Struct)"),
		GetCompareMethodName()
	);
}

FText UK2Node_Compare_PulldownStruct::GetKeywords() const
{
	return GetCompareMethodOperator();
}

void UK2Node_Compare_PulldownStruct::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, LhsPinName);
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, RhsPinName);
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
	return LOCTEXT("MenuCategory", "Pulldown Struct");
}

void UK2Node_Compare_PulldownStruct::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	Super::GetMenuActions(ActionRegistrar);

	const UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(IsValid(NodeSpawner));
		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

void UK2Node_Compare_PulldownStruct::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
	
	UK2Node_CallFunction* FunctionNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	if (ensure(IsValid(FunctionNode)))
	{
		FunctionNode->FunctionReference.SetExternalMember(GetFunctionName(), UPulldownStructFunctionLibrary::StaticClass());
		FunctionNode->AllocateDefaultPins();

		{
			UEdGraphPin* IntermediateLhsPin = FunctionNode->FindPinChecked(LhsPinName);
			UEdGraphPin* SourceLhsPin = GetLhsPin();
			if (ensure(IntermediateLhsPin != nullptr && SourceLhsPin != nullptr))
			{
				IntermediateLhsPin->PinType = SourceLhsPin->PinType;
				IntermediateLhsPin->PinType.PinSubCategoryObject = SourceLhsPin->PinType.PinSubCategoryObject;
				CompilerContext.MovePinLinksToIntermediate(*SourceLhsPin, *IntermediateLhsPin);
			}
		}
		{
			UEdGraphPin* IntermediateRhsPin = FunctionNode->FindPinChecked(RhsPinName);
			UEdGraphPin* SourceRhsPin = GetRhsPin();
			if (ensure(IntermediateRhsPin != nullptr && SourceRhsPin != nullptr))
			{
				IntermediateRhsPin->PinType = SourceRhsPin->PinType;
				IntermediateRhsPin->PinType.PinSubCategoryObject = SourceRhsPin->PinType.PinSubCategoryObject;
				CompilerContext.MovePinLinksToIntermediate(*SourceRhsPin, *IntermediateRhsPin);
			}
		}
		{
			UEdGraphPin* IntermediateReturnValuePin = FunctionNode->GetReturnValuePin();
			UEdGraphPin* SourceReturnValuePin = GetReturnValuePin();
			if (ensure(IntermediateReturnValuePin != nullptr && SourceReturnValuePin != nullptr))
			{
				CompilerContext.MovePinLinksToIntermediate(*SourceReturnValuePin, *IntermediateReturnValuePin);
			}
		}
	}
	
	BreakAllNodeLinks();
}

void UK2Node_Compare_PulldownStruct::PostReconstructNode()
{
	Super::PostReconstructNode();

	RefreshArgumentStructType();
}

void UK2Node_Compare_PulldownStruct::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);

	RefreshArgumentStructType();
}

bool UK2Node_Compare_PulldownStruct::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const
{
	if (!ensure(MyPin != nullptr && OtherPin != nullptr))
	{
		return true;
	}
	
	bool bWasDisallowed = true;
	EnumerateArgumentPins(
		[&](const UEdGraphPin& ArgumentPin) -> bool
		{
			if (MyPin != &ArgumentPin)
			{
				return true;
			}
			
			if (MyPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
			{
				return true;
			}
			
			if (OtherPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
			{
				if (const UScriptStruct* ConnectionType = Cast<UScriptStruct>(OtherPin->PinType.PinSubCategoryObject.Get()))
				{
					bWasDisallowed = !(
						PulldownBuilder::FPulldownBuilderUtils::IsPulldownStruct(ConnectionType) ||
						PulldownBuilder::FPulldownBuilderUtils::IsNativeLessPulldownStruct(ConnectionType)
					);
				}
			}
			else if (OtherPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
			{
				bWasDisallowed = false;
			}

			if (bWasDisallowed)
			{
				OutReason = LOCTEXT("DisallowedReason", "The type of the structure specified in the argument must be a structure that inherits PulldownStructBase or NativeLessPulldownStruct.").ToString();
				return false;
			}

			return true;
		}
	);
	
	return Super::IsConnectionDisallowed(MyPin, OtherPin, OutReason);
}

UEdGraphPin* UK2Node_Compare_PulldownStruct::GetLhsPin() const
{
	UEdGraphPin* Pin = FindPinChecked(LhsPinName);
	check(Pin != nullptr && Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_Compare_PulldownStruct::GetRhsPin() const
{
	UEdGraphPin* Pin = FindPinChecked(RhsPinName);
	check(Pin != nullptr && Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_Compare_PulldownStruct::GetReturnValuePin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	check(Pin != nullptr && Pin->Direction == EGPD_Output);
	return Pin;
}

void UK2Node_Compare_PulldownStruct::EnumerateArgumentPins(const TFunction<bool(UEdGraphPin& ArgumentPin)>& Predicate) const
{
	TArray<UEdGraphPin*, TInlineAllocator<2>> ArgumentPins { GetLhsPin(), GetRhsPin() };
	for (auto* ArgumentPin : ArgumentPins)
	{
		if (ArgumentPin == nullptr)
		{
			continue;
		}

		if (!Predicate(*ArgumentPin))
		{
			break;
		}
	}
}

UScriptStruct* UK2Node_Compare_PulldownStruct::GetArgumentStructType() const
{
	UScriptStruct* ArgumentStructType = nullptr;
	
	EnumerateArgumentPins(
		[&](const UEdGraphPin& ArgumentPin) -> bool
		{
			if (ArgumentPin.LinkedTo.Num() == 0)
			{
				return true;
			}

			const TArray<UEdGraphPin*>& LinkedTo = ArgumentPin.LinkedTo;
			ArgumentStructType = Cast<UScriptStruct>(LinkedTo[0]->PinType.PinSubCategoryObject.Get());
			for (int32 LinkIndex = 1; LinkIndex < LinkedTo.Num(); LinkIndex++)
			{
				const UEdGraphPin* Link = LinkedTo[LinkIndex];
				UScriptStruct* LinkType = Cast<UScriptStruct>(Link->PinType.PinSubCategoryObject.Get());
				if (ArgumentStructType != nullptr && ArgumentStructType->IsChildOf(LinkType))
				{
					ArgumentStructType = LinkType;
				}
			}

			return (ArgumentStructType == nullptr);
		}
	);

	return ArgumentStructType;
}

void UK2Node_Compare_PulldownStruct::RefreshArgumentStructType()
{
	const UScriptStruct* OldArgumentStructType = nullptr;
	EnumerateArgumentPins(
		[&](const UEdGraphPin& ArgumentPin) -> bool
		{
			if (ArgumentPin.PinType.PinCategory != UEdGraphSchema_K2::PC_Struct)
			{
				return true;
			}

			OldArgumentStructType = Cast<UScriptStruct>(ArgumentPin.PinType.PinSubCategoryObject.Get());
			if (IsValid(OldArgumentStructType))
			{
				return false;
			}

			return true;
		}
	);

	UScriptStruct* NewArgumentStructType = GetArgumentStructType();
	if (NewArgumentStructType != OldArgumentStructType)
	{
		EnumerateArgumentPins(
			[&](UEdGraphPin& ArgumentPin) -> bool
			{
				if (ArgumentPin.SubPins.Num() > 0)
				{
					GetSchema()->RecombinePin(&ArgumentPin);
				}

				ArgumentPin.PinType.PinCategory = (
					IsValid(NewArgumentStructType) ?
					UEdGraphSchema_K2::PC_Struct :
					UEdGraphSchema_K2::PC_Wildcard
				);
				ArgumentPin.PinType.PinSubCategoryObject = NewArgumentStructType;
				
				return true;
			}
		);

		CachedNodeTitle.Clear();
	}
}

#undef LOCTEXT_NAMESPACE
