// Copyright 2021-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownStructNodes/K2Nodes/K2Node_MakeLiteral_PulldownStruct.h"
#include "K2Node_MakeLiteral_NativeLessPulldownStruct.generated.h"

class UEdGraphSchema_K2;
class UK2Node_MakeStruct;
class FKismetCompilerContext;

/**
 * A node class that constructs a literal value of an FNativeLessPulldownStruct.
 * Carries an additional PulldownSource so that the constructed struct points at the correct PulldownContents.
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
	virtual bool ApplyDefaultsToMakeStructNode(
		FKismetCompilerContext& CompilerContext,
		const UEdGraphSchema_K2* K2Schema,
		UK2Node_MakeStruct* MakeStructNode
	) const override;
	virtual bool IsTargetStruct(const UScriptStruct* Struct) const override;
	// End of UK2Node_MakeLiteral_PulldownStruct interface.

	// Returns the PulldownSource of the constructed pull-down struct.
	const FName& GetPulldownSource() const;

	// Sets the PulldownSource of the constructed pull-down struct.
	void SetPulldownSource(const FName& InPulldownSource);

protected:
	// The PulldownContents name to embed as the PulldownSource of the constructed pull-down struct.
	UPROPERTY(EditAnywhere, Category = "Native Less Pulldown Struct")
	FName PulldownSource;
};
