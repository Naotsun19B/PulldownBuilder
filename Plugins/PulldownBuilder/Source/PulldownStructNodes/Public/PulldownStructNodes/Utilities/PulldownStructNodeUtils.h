// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UEdGraphSchema_K2;
class UEdGraphPin;
class UK2Node;
class FKismetCompilerContext;

namespace PulldownBuilder
{
	/**
	 * A utility class that defines the processes used in pull-down struct nodes.
	 */
	class PULLDOWNSTRUCTNODES_API FPulldownStructNodeUtils
	{
	public:
		// Finds the FName pin from the pull-down struct pins.
		static UEdGraphPin* FindNameSubPin(const UEdGraphSchema_K2* K2Schema, UEdGraphPin* PulldownStructPin, const FName& SubPinName);
		
		// Connects the input pin of the pull-down struct and the input pin of FName.
		static bool LinkPulldownStructPinToNamePin(
			UK2Node* SourceNode,
			FKismetCompilerContext& CompilerContext,
			const UEdGraphSchema_K2* K2Schema,
			UEdGraphPin* PulldownStructPin,
			UEdGraphPin* SelectedValueNamePin,
			UEdGraphPin* PulldownSourceNamePin = nullptr
		);
	};
}
