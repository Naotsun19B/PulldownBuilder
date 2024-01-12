// Copyright 2021-2024 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/Utilities/PulldownStructNodeUtils.h"
#if WITH_EDITOR
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#endif
#include "PulldownStruct/PulldownStructBase.h"
#include "PulldownStruct/NativeLessPulldownStruct.h"
#include "EdGraphSchema_K2.h"
#include "EdGraph/EdGraphPin.h"
#include "K2Node.h"
#include "KismetCompiler.h"
#include "K2Node_BreakStruct.h"

namespace PulldownBuilder
{
	UEdGraphPin* FPulldownStructNodeUtils::FindNameSubPin(const UEdGraphSchema_K2* K2Schema, UEdGraphPin* PulldownStructPin, const FName& SubPinName)
	{
		check(IsValid(K2Schema) && PulldownStructPin != nullptr);

		if (K2Schema->CanSplitStructPin(*PulldownStructPin))
		{
			K2Schema->SplitPin(PulldownStructPin, false);
		}

		UEdGraphPin* const* SelectedValuePinPtr = PulldownStructPin->SubPins.FindByPredicate(
			[&SubPinName](const UEdGraphPin* SubPin) -> bool
			{
				if (SubPin == nullptr)
				{
					return false;
				}

				const FString& PinName = SubPin->GetName();
				TArray<FString> ParsedName;
				PinName.ParseIntoArray(ParsedName, TEXT("_"));
				if (ParsedName.Num() == 0)
				{
					return false;
				}

				return ParsedName.Last().Equals(SubPinName.ToString());
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
		UEdGraphPin* SelectedValueNamePin,
		UEdGraphPin* PulldownSourceNamePin /* = nullptr */
	)
	{
		check(IsValid(SourceNode) && IsValid(K2Schema) && PulldownStructPin != nullptr && SelectedValueNamePin != nullptr);

		// Both pins must be input pins.
		if (!ensure(PulldownStructPin->Direction == EGPD_Input) || !ensure(SelectedValueNamePin->Direction == EGPD_Input))
		{
			return false;
		}

		// The pull-down struct pin must be a struct type.
		if (!ensure(PulldownStructPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct))
		{
			return false;
		}

		// The name pin must be of type FName.
		if (!ensure(SelectedValueNamePin->PinType.PinCategory == UEdGraphSchema_K2::PC_Name))
		{
			return false;
		}
		
		// Need specific type information for struct pin.
		auto* PulldownStruct = Cast<UScriptStruct>(PulldownStructPin->PinType.PinSubCategoryObject);
#if WITH_EDITOR
		if (!ensure(FPulldownBuilderUtils::IsPulldownStruct(PulldownStruct, false)))
#else
		if (!IsValid(PulldownStruct))
#endif
		{
			return false;
		}

		// If need to connect the pulldown source pin as well, it must be an FName type input pin.
		const bool bWithPulldownSource = (PulldownSourceNamePin != nullptr);
		if (bWithPulldownSource)
		{
			if (!ensure(PulldownSourceNamePin->Direction == EGPD_Input && PulldownSourceNamePin->PinType.PinCategory == UEdGraphSchema_K2::PC_Name))
			{
				return false;
			}
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

			{
				UEdGraphPin* StructPin = BreakNode->FindPinChecked(PulldownStruct->GetFName(), EGPD_Input);
				const FPinConnectionResponse& Response = CompilerContext.MovePinLinksToIntermediate(
					*PulldownStructPin,
					*StructPin
				);
				if (!Response.CanSafeConnect())
				{
					bConnectionResult = false;
				}
			}
			
			{
				UEdGraphPin* SelectedValuePin = BreakNode->FindPin(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue), EGPD_Output);
				if (!K2Schema->TryCreateConnection(SelectedValuePin, SelectedValueNamePin))
				{
					bConnectionResult = false;
				}
			}

			if (bWithPulldownSource)
			{
				UEdGraphPin* PulldownSourcePin = BreakNode->FindPin(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource), EGPD_Output);
				if (!K2Schema->TryCreateConnection(PulldownSourcePin, PulldownSourceNamePin))
				{
					bConnectionResult = false;
				}
			}
		}
		else
		{
			{
				UEdGraphPin* SelectedValuePin = FindNameSubPin(K2Schema, PulldownStructPin, GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue));
				check(SelectedValuePin != nullptr);
				const FPinConnectionResponse& Response = CompilerContext.MovePinLinksToIntermediate(
					*SelectedValuePin,
					*SelectedValueNamePin
				);
				if (!Response.CanSafeConnect())
				{
					bConnectionResult = false;
				}
			}

			if (bWithPulldownSource)
			{
				UEdGraphPin* PulldownSourcePin = FindNameSubPin(K2Schema, PulldownStructPin, GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource));
				check(PulldownSourcePin != nullptr);
				const FPinConnectionResponse& Response = CompilerContext.MovePinLinksToIntermediate(
					*PulldownSourcePin,
					*PulldownSourceNamePin
				);
				if (!Response.CanSafeConnect())
				{
					bConnectionResult = false;
				}
			}
		}

		return bConnectionResult;
	}
}
