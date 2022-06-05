// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "K2Node_Equal_PulldownStruct.generated.h"

/**
 * Compares FPulldownStructBase::SelectedValue between pulldown structures of the same type and returns the result.
 */
UCLASS(meta = (Keywords = "=="))
class PULLDOWNSTRUCTNODES_API UK2Node_Equal_PulldownStruct : public UK2Node
{
	GENERATED_BODY()

public:
	// Defines the name of the pins contained in this node.
	static const FName LhsPinName;
	static const FName RhsPinName;
	
public:
	// UEdGraphNode interface.
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
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
	
protected:
	// A cache of text for the title of this node.
	FNodeTextCache CachedNodeTitle;
};
