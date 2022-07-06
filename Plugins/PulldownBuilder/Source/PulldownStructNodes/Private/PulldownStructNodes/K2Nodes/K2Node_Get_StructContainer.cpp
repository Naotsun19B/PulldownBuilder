// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/K2Nodes/K2Node_Get_StructContainer.h"
#include "PulldownBuilder/Types/StructContainer.h"
#include "EdGraphSchema_K2.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "K2Node_CallFunction.h"
#include "K2Node_IfThenElse.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "K2Node_Get_StructContainer"

const FName UK2Node_Get_StructContainer::FailedPinName		= TEXT("Failed");
const FName UK2Node_Get_StructContainer::TargetPinName		= TEXT("Target");
const FName UK2Node_Get_StructContainer::StructDataPinName	= TEXT("StructData");

FText UK2Node_Get_StructContainer::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType != ENodeTitleType::MenuTitle)
	{
		if (const UScriptStruct* ValueStruct = GetStructTypeFromStructDataPin())
		{
			if (CachedNodeTitle.IsOutOfDate(this))
			{
				CachedNodeTitle.SetCachedText(
					FText::Format(
						LOCTEXT("NodeTitleFormat", "Get {StructName} From Struct Container"),
						ValueStruct->GetDisplayNameText()
					),
					this
				);
			}
		
			return CachedNodeTitle;
		}
	}
	
	return LOCTEXT("NodeTitle", "Get (Struct Container)");
}

FText UK2Node_Get_StructContainer::GetKeywords() const
{
	return LOCTEXT("Keywords", "Get (Struct Container)");
}

void UK2Node_Get_StructContainer::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, FailedPinName);
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FStructContainer::StaticStruct(), TargetPinName);
	if (UEdGraphPin* StructDataPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, StructDataPinName))
	{
		StructDataPin->PinFriendlyName = FText::FromName(UEdGraphSchema_K2::PN_ReturnValue);
	}
}

FText UK2Node_Get_StructContainer::GetMenuCategory() const
{
	return LOCTEXT("MenuCategory", "Struct Container");
}

void UK2Node_Get_StructContainer::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
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

void UK2Node_Get_StructContainer::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
	
	UK2Node_CallFunction* FunctionNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	FunctionNode->FunctionReference.SetExternalMember(
		GET_FUNCTION_NAME_CHECKED(UStructContainerFunctionLibrary, Get_StructContainer),
		UStructContainerFunctionLibrary::StaticClass()
	);
	FunctionNode->AllocateDefaultPins();

	{
		UEdGraphPin* IntermediateTargetPin = FunctionNode->FindPinChecked(TargetPinName);
		UEdGraphPin* SourceTargetPin = GetTargetPin();
		if (ensure(IntermediateTargetPin != nullptr && SourceTargetPin != nullptr))
		{
			CompilerContext.MovePinLinksToIntermediate(*SourceTargetPin, *IntermediateTargetPin);
		}
	}
	{
		UEdGraphPin* IntermediateStructDataPin = FunctionNode->FindPinChecked(StructDataPinName);
		UEdGraphPin* SourceStructDataPin = GetStructDataPin();
		if (ensure(IntermediateStructDataPin != nullptr && SourceStructDataPin != nullptr))
		{
			IntermediateStructDataPin->PinType = SourceStructDataPin->PinType;
            IntermediateStructDataPin->PinType.PinSubCategoryObject = SourceStructDataPin->PinType.PinSubCategoryObject;
            CompilerContext.MovePinLinksToIntermediate(*SourceStructDataPin, *IntermediateStructDataPin);
		}
	}
	{
		UEdGraphPin* IntermediateExecPin = FunctionNode->GetExecPin();
		UEdGraphPin* SourceExecPin = GetExecPin();
		if (ensure(IntermediateExecPin != nullptr && SourceExecPin != nullptr))
		{
			CompilerContext.MovePinLinksToIntermediate(*SourceExecPin, *IntermediateExecPin);
		}
	}
	if (auto* BranchNode = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph))
	{
		BranchNode->AllocateDefaultPins();
		
		{
			UEdGraphPin* IntermediateThenPin = FunctionNode->GetThenPin();
			UEdGraphPin* IntermediateReturnPin = FunctionNode->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
			if (ensure(IntermediateThenPin != nullptr && IntermediateReturnPin != nullptr))
			{
				IntermediateThenPin->MakeLinkTo(BranchNode->GetExecPin());
				IntermediateReturnPin->MakeLinkTo(BranchNode->GetConditionPin());
			}
		}
		
		{
			UEdGraphPin* BranchNodeThenPin = BranchNode->GetThenPin();
			UEdGraphPin* SourceThenPin = GetThenPin();
			if (ensure(BranchNodeThenPin != nullptr && SourceThenPin != nullptr))
			{
				CompilerContext.MovePinLinksToIntermediate(*SourceThenPin, *BranchNodeThenPin);
			}
		}
		{
			UEdGraphPin* BranchNodeFailedPin = BranchNode->GetElsePin();
			UEdGraphPin* SourceFailedPin = GetFailedPin();
			if (ensure(BranchNodeFailedPin != nullptr && SourceFailedPin != nullptr))
			{
				CompilerContext.MovePinLinksToIntermediate(*SourceFailedPin, *BranchNodeFailedPin);
			}
		}
	}
	
	BreakAllNodeLinks();
}

void UK2Node_Get_StructContainer::PostReconstructNode()
{
	Super::PostReconstructNode();
	
	RefreshStructDataPinType();
}

void UK2Node_Get_StructContainer::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);

	RefreshStructDataPinType();
}

bool UK2Node_Get_StructContainer::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const
{
	if (!ensure(MyPin != nullptr && OtherPin != nullptr))
	{
		return true;
	}
	
	bool bWasDisallowed = true;
	if (MyPin == GetStructDataPin())
	{
		if (MyPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
		{
			if (OtherPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
			{
				bWasDisallowed = false;
			}
			else
			{
				bWasDisallowed = (OtherPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Struct);
			}
		}
	}
	
	if (bWasDisallowed)
	{
		OutReason = LOCTEXT("DisallowedReason", "Struct Data pin can only be used with structure types.").ToString();
		return false;
	}
	
	return Super::IsConnectionDisallowed(MyPin, OtherPin, OutReason);
}

UEdGraphPin* UK2Node_Get_StructContainer::GetThenPin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_Get_StructContainer::GetFailedPin() const
{
	UEdGraphPin* Pin = FindPinChecked(FailedPinName);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_Get_StructContainer::GetTargetPin() const
{
	UEdGraphPin* Pin = FindPinChecked(TargetPinName);
    check(Pin->Direction == EGPD_Input);
    return Pin;
}

UEdGraphPin* UK2Node_Get_StructContainer::GetStructDataPin() const
{
	UEdGraphPin* Pin = FindPinChecked(StructDataPinName);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UScriptStruct* UK2Node_Get_StructContainer::GetStructType() const
{
	const UEdGraphPin* StructDataPin = GetStructDataPin();
	if (StructDataPin != nullptr && StructDataPin->LinkedTo.Num() > 0)
	{
		const TArray<UEdGraphPin*>& LinkedTo = StructDataPin->LinkedTo;
		const UScriptStruct* StructType = Cast<UScriptStruct>(LinkedTo[0]->PinType.PinSubCategoryObject.Get());
		for (int32 LinkIndex = 1; LinkIndex < LinkedTo.Num(); LinkIndex++)
		{
			const UEdGraphPin* Link = LinkedTo[LinkIndex];
			UScriptStruct* LinkType = Cast<UScriptStruct>(Link->PinType.PinSubCategoryObject.Get());
			if (IsValid(StructType) && StructType->IsChildOf(LinkType))
			{
				return LinkType;
			}
		}
	}
	
	return nullptr;
}

UScriptStruct* UK2Node_Get_StructContainer::GetStructTypeFromStructDataPin() const
{
	const UEdGraphPin* StructDataPin = GetStructDataPin();
	if(StructDataPin != nullptr)
	{
		if(StructDataPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
		{
			return Cast<UScriptStruct>(StructDataPin->PinType.PinSubCategoryObject.Get());
		}
	}
	
	return nullptr;
}

void UK2Node_Get_StructContainer::RefreshStructDataPinType()
{
	UScriptStruct* NewStructType = GetStructType();
	const UScriptStruct* OldStructType = GetStructTypeFromStructDataPin();
	if (NewStructType != OldStructType)
	{
		if (UEdGraphPin* StructDataPin = GetStructDataPin())
		{
			if (StructDataPin->SubPins.Num() > 0)
			{
				if (const UEdGraphSchema* Schema = GetSchema())
				{
					Schema->RecombinePin(StructDataPin);
				}
			}
			
			StructDataPin->PinType.PinCategory = (
				IsValid(NewStructType) ?
				UEdGraphSchema_K2::PC_Struct :
				UEdGraphSchema_K2::PC_Wildcard
			);
			StructDataPin->PinType.PinSubCategoryObject = NewStructType;
		}
		
		CachedNodeTitle.Clear();
	}
}

#undef LOCTEXT_NAMESPACE
