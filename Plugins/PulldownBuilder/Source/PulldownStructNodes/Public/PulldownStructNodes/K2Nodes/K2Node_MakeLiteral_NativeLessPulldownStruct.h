// Copyright 2021-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownStructNodes/K2Nodes/K2Node_MakeLiteral_PulldownStruct.h"
#include "K2Node_MakeLiteral_NativeLessPulldownStruct.generated.h"

/**
 * A node class that constructs a literal value of an FNativeLessPulldownStruct.
 * Exposes a single FNativeLessPulldownStruct input pin; the native-less pulldown graph-pin
 * factory draws the PulldownSource and SelectedValue pickers directly on the pin's default-value widget.
 */
UCLASS()
class PULLDOWNSTRUCTNODES_API UK2Node_MakeLiteral_NativeLessPulldownStruct : public UK2Node_MakeLiteral_PulldownStruct
{
	GENERATED_BODY()

public:
	// Constructor.
	UK2Node_MakeLiteral_NativeLessPulldownStruct();

	// UEdGraphNode interface.
	virtual FText GetTooltipText() const override;
	// End of UEdGraphNode interface.

	// UK2Node_MakeLiteral_PulldownStruct interface.
	virtual bool ApplyInputPinDefaultsToMakeStructNode(
		FKismetCompilerContext& CompilerContext,
		const UEdGraphSchema_K2* K2Schema,
		UK2Node_MakeStruct* MakeStructNode,
		const UEdGraphPin* InputPin
	) const override;
	virtual bool IsTargetStruct(const UScriptStruct* Struct) const override;
	virtual bool HasLegacyDefaultValues() const override;
	virtual void ApplyLegacyDefaultValuesToString(FString& InOutDefaultValueString) const override;
	virtual void ClearLegacyDefaultValues() override;
	// End of UK2Node_MakeLiteral_PulldownStruct interface.

protected:
	// Legacy storage of the PulldownSource from before this node moved to an input-pin UX.
	// Read once during AllocateDefaultPins / PostLoad, then cleared. Kept as UPROPERTY so existing
	// assets still deserialize without losing data during the migration window.
	UPROPERTY()
	FName PulldownSource;
};
