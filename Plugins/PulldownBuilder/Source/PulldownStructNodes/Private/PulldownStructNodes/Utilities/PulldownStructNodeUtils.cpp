// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/Utilities/PulldownStructNodeUtils.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "EdGraphSchema_K2.h"
#include "EdGraph/EdGraphPin.h"
#include "K2Node.h"
#include "KismetCompiler.h"
#include "K2Node_BreakStruct.h"

namespace PulldownBuilder
{
	UEdGraphPin* FPulldownStructNodeUtils::FindSelectedValueSubPin(const UEdGraphSchema_K2* K2Schema, UEdGraphPin* PulldownStructPin)
	{
		check(IsValid(K2Schema) && PulldownStructPin != nullptr);

		K2Schema->SplitPin(PulldownStructPin, false);

		UEdGraphPin* const* SelectedValuePinPtr = PulldownStructPin->SubPins.FindByPredicate(
			[](const UEdGraphPin* SubPin) -> bool
			{
				if (SubPin == nullptr)
				{
					return false;
				}

				const FString& SubPinName = SubPin->GetName();
				TArray<FString> ParsedName;
				SubPinName.ParseIntoArray(ParsedName, TEXT("_"));
				if (ParsedName.Num() == 0)
				{
					return false;
				}

				return ParsedName.Last().Equals(GET_MEMBER_NAME_STRING_CHECKED(FPulldownStructBase, SelectedValue));
			}
		);
		if (SelectedValuePinPtr != nullptr)
		{
			return *SelectedValuePinPtr;
		}

		return nullptr;
	}

	bool FPulldownStructNodeUtils::LinkPulldownStructPinToNamePin(
		UK2Node* SourceNode,
		FKismetCompilerContext& CompilerContext,
		const UEdGraphSchema_K2* K2Schema,
		UEdGraphPin* PulldownStructPin,
		UEdGraphPin* NamePin
	)
	{
		check(IsValid(SourceNode) && IsValid(K2Schema) && PulldownStructPin != nullptr && NamePin != nullptr);

		// Both pins must be input pins.
		if (!ensure(PulldownStructPin->Direction == EGPD_Input) || !ensure(NamePin->Direction == EGPD_Input))
		{
			return false;
		}

		// The pull-down struct pin must be a struct type.
		if (!ensure(PulldownStructPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct))
		{
			return false;
		}

		// The name pin must be of type FName.
		if (!ensure(NamePin->PinType.PinCategory == UEdGraphSchema_K2::PC_Name))
		{
			return false;
		}
		
		// Need specific type information for struct pin.
		auto* PulldownStruct = Cast<UScriptStruct>(PulldownStructPin->PinType.PinSubCategoryObject);
		if (!ensure(IsValid(PulldownStruct)))
		{
			return false;
		}

		bool bConnectionResult = true;
		
		// If something is connected to the source pin, get the FPulldownStructBase::SelectedValue at the break node.
		// If the break node is not connected to anything, an error will occur,
		// so if the source pin is the default value, split the pin and extract the FPulldownStructBase::SelectedValue.
		if (PulldownStructPin->LinkedTo.IsValidIndex(0))
		{
			auto* BreakNode = CompilerContext.SpawnIntermediateNode<UK2Node_BreakStruct>(SourceNode, SourceNode->GetGraph());
			check(IsValid(BreakNode));
			BreakNode->StructType = PulldownStruct;
			BreakNode->bMadeAfterOverridePinRemoval = true;
			BreakNode->AllocateDefaultPins();

			UEdGraphPin* StructPin = BreakNode->FindPinChecked(PulldownStruct->GetFName(), EGPD_Input);
			const FPinConnectionResponse& Response = CompilerContext.MovePinLinksToIntermediate(
				*PulldownStructPin,
				*StructPin
			);
			if (!Response.CanSafeConnect())
			{
				bConnectionResult = false;
			}
			
			UEdGraphPin* SelectedValuePin = BreakNode->FindPinChecked(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue), EGPD_Output);
			if (!K2Schema->TryCreateConnection(SelectedValuePin, NamePin))
			{
				bConnectionResult = false;
			}
		}
		else
		{
			UEdGraphPin* SelectedValuePin = FindSelectedValueSubPin(K2Schema, PulldownStructPin);
			check(SelectedValuePin != nullptr);
			const FPinConnectionResponse& Response = CompilerContext.MovePinLinksToIntermediate(
				*SelectedValuePin,
				*NamePin
			);
			if (!Response.CanSafeConnect())
			{
				bConnectionResult = false;
			}
		}

		return bConnectionResult;
	}
}
