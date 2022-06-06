// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_Compare_PulldownStruct.generated.h"

/**
 * Base class for blueprint nodes that compare pulldown structures.
 */
UCLASS(Abstract)
class PULLDOWNSTRUCTNODES_API UK2Node_Compare_PulldownStruct : public UK2Node
{
	GENERATED_BODY()

public:
	// Defines the name of the pins contained in this node.
	static const FName LhsPinName;
	static const FName RhsPinName;
	
public:
	// UEdGraphNode interface.
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetKeywords() const override;
	virtual void AllocateDefaultPins() override;
	// End of UEdGraphNode interface.

	// UK2Node interface.
	virtual bool IsNodePure() const override;
	virtual bool ShouldDrawCompact() const override;
	virtual FText GetCompactNodeTitle() const override;
	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void PostReconstructNode() override;
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	// End of UK2Node interface.
	
protected:
	// Functions that get each pin contained in this node.
	UEdGraphPin* GetLhsPin() const;
	UEdGraphPin* GetRhsPin() const;
	UEdGraphPin* GetReturnValuePin() const;
	void EnumerateArgumentPins(const TFunction<bool(UEdGraphPin& ArgumentPin)>& Predicate) const;

	// Returns the type of the structure specified in the argument pins.
	UScriptStruct* GetArgumentStructType() const;

	// Refresh so that the types of the two input pins are the same structure type.
	void RefreshArgumentStructType();

	// Returns the name of the comparison method used in the node title, etc.
	virtual FText GetCompareMethodName() const PURE_VIRTUAL(UK2Node_Compare_PulldownStruct::GetCompareMethodName, { return FText::GetEmpty(); });

	// Returns the operator of the comparison method used in the node title, etc.
	virtual FText GetCompareMethodOperator() const PURE_VIRTUAL(UK2Node_Compare_PulldownStruct::GetCompareMethodOperator, { return FText::GetEmpty(); });
	
	// Get the name of the function to actually execute.
	virtual FName GetFunctionName() const PURE_VIRTUAL(UK2Node_Compare_PulldownStruct::GetFunctionName, { return NAME_None; });
	
protected:
	// A cache of text for the title of this node.
	FNodeTextCache CachedNodeTitle;
};
