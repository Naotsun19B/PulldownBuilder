// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/K2Nodes/K2Node_Get_StructContainer.h"
#include "PulldownBuilder/Types/StructContainer.h"
#include "PulldownBuilder/Utilities/StructContainerFunctionLibrary.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "EdGraphSchema_K2.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_Get_StructContainer)
#endif

#define LOCTEXT_NAMESPACE "K2Node_Get_StructContainer"

const FName UK2Node_Get_StructContainer::TargetPinName		= TEXT("Target");
const FName UK2Node_Get_StructContainer::StructDataPinName	= TEXT("StructData");

FText UK2Node_Get_StructContainer::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (ArePinsAllocated() && TitleType != ENodeTitleType::MenuTitle)
	{
		if (const UScriptStruct* ValueStruct = GetStructType())
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
	
	return LOCTEXT("Title", "Get (Struct Container)");
}

FText UK2Node_Get_StructContainer::GetTooltipText() const
{
	return LOCTEXT("Tooltip", "Returns the data of the struct stored in the container.");
}

FText UK2Node_Get_StructContainer::GetKeywords() const
{
	return LOCTEXT("Keywords", "Get (Struct Container)");
}

FSlateIcon UK2Node_Get_StructContainer::GetIconAndTint(FLinearColor& OutColor) const
{
	const UFunction* Function = FindUField<UFunction>(
		UStructContainerFunctionLibrary::StaticClass(),
		GET_FUNCTION_NAME_CHECKED(UStructContainerFunctionLibrary, Get_StructContainer)
	);
	check(IsValid(Function));
	return UK2Node_CallFunction::GetPaletteIconForFunction(Function, OutColor);
}

void UK2Node_Get_StructContainer::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FStructContainer::StaticStruct(), TargetPinName);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, StructDataPinName);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, UEdGraphSchema_K2::PN_ReturnValue);
}

bool UK2Node_Get_StructContainer::IsNodePure() const
{
	return true;
}

FText UK2Node_Get_StructContainer::GetMenuCategory() const
{
	return LOCTEXT("MenuCategory", "Struct Container");
}

void UK2Node_Get_StructContainer::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(ActionKey);
		check(IsValid(NodeSpawner));
		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

void UK2Node_Get_StructContainer::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
	
	UK2Node_CallFunction* GetStructContainerNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	check(IsValid(GetStructContainerNode));
	GetStructContainerNode->FunctionReference.SetExternalMember(
		GET_FUNCTION_NAME_CHECKED(UStructContainerFunctionLibrary, Get_StructContainer),
		UStructContainerFunctionLibrary::StaticClass()
	);
	GetStructContainerNode->AllocateDefaultPins();

	{
		UEdGraphPin* IntermediateTargetPin = GetStructContainerNode->FindPinChecked(TargetPinName);
		UEdGraphPin* SourceTargetPin = GetTargetPin();
		IntermediateTargetPin->PinType = SourceTargetPin->PinType;
		IntermediateTargetPin->PinType.PinSubCategoryObject = SourceTargetPin->PinType.PinSubCategoryObject;
		CompilerContext.MovePinLinksToIntermediate(*SourceTargetPin, *IntermediateTargetPin);
	}
	{
		UEdGraphPin* IntermediateStructDataPin = GetStructContainerNode->FindPinChecked(StructDataPinName);
		UEdGraphPin* SourceStructDataPin = GetStructDataPin();
		IntermediateStructDataPin->PinType = SourceStructDataPin->PinType;
		IntermediateStructDataPin->PinType.PinSubCategoryObject = SourceStructDataPin->PinType.PinSubCategoryObject;
		CompilerContext.MovePinLinksToIntermediate(*SourceStructDataPin, *IntermediateStructDataPin);
	}
	{
		UEdGraphPin* IntermediateReturnValuePin = GetStructContainerNode->GetReturnValuePin();
		UEdGraphPin* SourceReturnValuePin = GetReturnValuePin();
		CompilerContext.MovePinLinksToIntermediate(*SourceReturnValuePin, *IntermediateReturnValuePin);
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
		OutReason = LOCTEXT("DisallowedReason", "Struct Data pin can only be used with struct types.").ToString();
		return false;
	}
	
	return Super::IsConnectionDisallowed(MyPin, OtherPin, OutReason);
}

bool UK2Node_Get_StructContainer::ArePinsAllocated() const
{
	static const TArray<FName, TInlineAllocator<3>> PinNames = {
		TargetPinName, StructDataPinName, UEdGraphSchema_K2::PN_ReturnValue
	};
	for (const auto& PinName : PinNames)
	{
		if (FindPin(PinName) == nullptr)
		{
			return false;
		}
	}
	
	return true;
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

UEdGraphPin* UK2Node_Get_StructContainer::GetReturnValuePin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	check(Pin != nullptr && Pin->Direction == EGPD_Output);
	return Pin;
}

UScriptStruct* UK2Node_Get_StructContainer::GetStructType() const
{
	const UEdGraphPin* StructDataPin = GetStructDataPin();
	if (StructDataPin == nullptr)
	{
		return nullptr;
	}

	const TArray<UEdGraphPin*>& LinkedTo = StructDataPin->LinkedTo;
	if (LinkedTo.Num() == 0)
	{
		return nullptr;
	}
	
	auto* StructType = Cast<UScriptStruct>(LinkedTo[0]->PinType.PinSubCategoryObject.Get());
	for (int32 LinkIndex = 1; LinkIndex < LinkedTo.Num(); LinkIndex++)
	{
		const UEdGraphPin* Link = LinkedTo[LinkIndex];
		auto* LinkType = Cast<UScriptStruct>(Link->PinType.PinSubCategoryObject.Get());
		if (StructType != nullptr && StructType->IsChildOf(LinkType))
		{
			StructType = LinkType;
		}
	}

	return StructType;
}

void UK2Node_Get_StructContainer::RefreshStructDataPinType()
{
	UEdGraphPin* StructDataPin = GetStructDataPin();
	if (StructDataPin == nullptr)
	{
		return;
	}

	const auto* OldStructType = Cast<UScriptStruct>(StructDataPin->PinType.PinSubCategoryObject.Get());
	UScriptStruct* NewStructType = GetStructType();
	if (NewStructType != OldStructType)
	{
		if (StructDataPin->SubPins.Num() > 0)
		{
			GetSchema()->RecombinePin(StructDataPin);
		}

		StructDataPin->PinType.PinCategory = (
			IsValid(NewStructType) ?
			UEdGraphSchema_K2::PC_Struct :
			UEdGraphSchema_K2::PC_Wildcard
		);
		StructDataPin->PinType.PinSubCategoryObject = NewStructType;

		CachedNodeTitle.Clear();
	}
}

#undef LOCTEXT_NAMESPACE
