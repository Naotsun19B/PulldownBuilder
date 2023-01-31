// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "K2Node_Get_StructContainer.generated.h"

/**
 * Returns the data of the struct stored in the container..
 */
UCLASS()
class PULLDOWNSTRUCTNODES_API UK2Node_Get_StructContainer : public UK2Node
{
	GENERATED_BODY()

public:
	// Defines the name of the pins contained in this node.
	static const FName TargetPinName;
	static const FName StructDataPinName;
	
public:
	// UEdGraphNode interface.
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetKeywords() const override;
	virtual void AllocateDefaultPins() override;
	// End of UEdGraphNode interface.

	// UK2Node interface.
	virtual bool IsNodePure() const override;
	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void PostReconstructNode() override;
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	// End of UK2Node interface.
	
protected:
	// Returns whether the node's pins are already allocated.
	bool ArePinsAllocated() const;
	
	// Functions that get each pin contained in this node.
	UEdGraphPin* GetTargetPin() const;
	UEdGraphPin* GetStructDataPin() const;
	UEdGraphPin* GetReturnValuePin() const;

	// Returns the type of struct specified by the struct data pin.
	UScriptStruct* GetStructType() const;

	// Refresh the struct data pin type to be the struct type of the connected pin.
	void RefreshStructDataPinType();
	
protected:
	// A cache of text for the title of this node.
	FNodeTextCache CachedNodeTitle;
};
