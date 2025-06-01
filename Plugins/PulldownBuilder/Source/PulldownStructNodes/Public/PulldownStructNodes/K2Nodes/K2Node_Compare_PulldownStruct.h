// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "K2Node_Compare_PulldownStruct.generated.h"

class UBlueprintNodeSpawner;

/**
 * A compare node class that compares the FPulldownStructBase::SelectedValue in the pull-down structs and returns the result.
 */
UCLASS(Abstract)
class PULLDOWNSTRUCTNODES_API UK2Node_Compare_PulldownStruct : public UK2Node
{
	GENERATED_BODY()

public:
	// Defines the name of the pins contained in this node.
	static const FName LhsPinName;
	static const FName RhsPinName;
	static const FName CompareNodeLhsPinName;
	static const FName CompareNodeRhsPinName;
	
public:
	// Constructor.
	UK2Node_Compare_PulldownStruct();
	
	// UEdGraphNode interface.
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetKeywords() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
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
	virtual bool ShouldShowNodeProperties() const override;
	// End of UK2Node interface.

	// Returns the pull-down struct to compare on this node.
	UScriptStruct* GetPulldownStruct() const;

	// Sets the pull-down struct to compare on this node.
	void SetPulldownStruct(UScriptStruct* NewPulldownStruct);
	
	// Returns whether to compare FNativeLessPulldownStruct::PulldownSource when comparing FNativeLessPulldownStruct.
	bool ShouldStrictComparison() const;
	
	// Sets whether to compare FNativeLessPulldownStruct::PulldownSource when comparing FNativeLessPulldownStruct.
	void SetShouldStrictComparison(const bool bNewState);
	
protected:
	// Returns the name of the comparison method used in the node title, etc.
	virtual FText GetCompareMethodName() const PURE_VIRTUAL(UK2Node_Compare_PulldownStruct::GetCompareMethodName, return FText::GetEmpty(););

	// Returns the operator of the comparison method used in the node title, etc.
	virtual FText GetCompareMethodOperator() const PURE_VIRTUAL(UK2Node_Compare_PulldownStruct::GetCompareMethodOperator, return FText::GetEmpty(););
	
	// Gets the name of the function to actually execute.
	virtual FName GetFunctionName() const PURE_VIRTUAL(UK2Node_Compare_PulldownStruct::GetFunctionName, return NAME_None;);
	
private:
	// Called when creating an instance of this node for each pull-down structs.
	UBlueprintNodeSpawner* HandleOnMakeStructSpawner(const UScriptStruct* Struct) const;
	
protected:
	// The pull-down struct to compare on this node.
	UPROPERTY()
	UScriptStruct* PulldownStruct;

	// Whether to compare FNativeLessPulldownStruct::PulldownSource when comparing FNativeLessPulldownStruct.
	UPROPERTY(EditAnywhere, Category = "Native Less Pulldown Struct")
	bool bStrictComparison;

	// The cache of text for the title of this node.
	FNodeTextCache CachedNodeTitle;

	// The cache of text for the tooltip of this node.
	FNodeTextCache CachedNodeTooltip;
	
	// The cache of text for the category of this node.
	FNodeTextCache CachedNodeCategory;
};
