// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "K2Node_Compare_PulldownStruct.generated.h"

class UBlueprintNodeSpawner;

/**
 * Compares FPulldownStructBase::SelectedValue between pull-down structs of the same type and returns values comparison result.
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
	virtual void PreloadRequiredAssets() override;
	// End of UK2Node interface.

protected:
	// Returns the name of the comparison method used in the node title, etc.
	virtual FText GetCompareMethodName() const PURE_VIRTUAL(UK2Node_Compare_PulldownStruct::GetCompareMethodName, return FText::GetEmpty(););

	// Returns the operator of the comparison method used in the node title, etc.
	virtual FText GetCompareMethodOperator() const PURE_VIRTUAL(UK2Node_Compare_PulldownStruct::GetCompareMethodOperator, return FText::GetEmpty(););
	
	// Get the name of the function to actually execute.
	virtual FName GetFunctionName() const PURE_VIRTUAL(UK2Node_Compare_PulldownStruct::GetFunctionName, return NAME_None;);
	
private:
	// Called when creating an instance of this node for each pull-down structs.
	UBlueprintNodeSpawner* HandleOnMakeStructSpawner(const UScriptStruct* Struct) const;
	
private:
	// A pull-down struct to compare on this node.
	UPROPERTY()
	UScriptStruct* PulldownStruct;

	// A cache of text for the title of this node.
	FNodeTextCache CachedNodeTitle;
};
