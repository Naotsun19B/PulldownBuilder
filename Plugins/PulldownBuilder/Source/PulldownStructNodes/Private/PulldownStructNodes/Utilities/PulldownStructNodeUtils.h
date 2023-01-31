// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UEdGraphSchema_K2;
class UEdGraphPin;
class UK2Node;
class FKismetCompilerContext;

namespace PulldownBuilder
{
	/**
	 * Utility function library class that defines the processes used in pulldown struct nodes.
	 */
	class PULLDOWNSTRUCTNODES_API FPulldownStructNodeUtils
	{
	public:
		// Finds the pin of FPulldownStructBase::SelectedValue from the pull-down struct pins.
		static UEdGraphPin* FindSelectedValueSubPin(const UEdGraphSchema_K2* K2Schema, UEdGraphPin* PulldownStructPin);

		// Connect the input pin of the pulldown struct and the input pin of FName.
		static bool LinkPulldownStructPinToNamePin(
			UK2Node* SourceNode,
			FKismetCompilerContext& CompilerContext,
			const UEdGraphSchema_K2* K2Schema,
			UEdGraphPin* PulldownStructPin,
			UEdGraphPin* NamePin
		);
	};
}
