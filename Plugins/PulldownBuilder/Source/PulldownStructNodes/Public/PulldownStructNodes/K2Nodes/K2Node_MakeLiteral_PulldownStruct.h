// Copyright 2021-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "K2Node_MakeLiteral_PulldownStruct.generated.h"

class UBlueprintNodeSpawner;
class UEdGraphSchema_K2;
class UK2Node_MakeStruct;
class FKismetCompilerContext;

/**
 * A node class that constructs a literal value of a pull-down struct from a SelectedValue chosen on the node.
 */
UCLASS()
class PULLDOWNSTRUCTNODES_API UK2Node_MakeLiteral_PulldownStruct : public UK2Node
{
	GENERATED_BODY()

public:
	// Constructor.
	UK2Node_MakeLiteral_PulldownStruct();

	// UEdGraphNode interface.
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetKeywords() const override;
	virtual void AllocateDefaultPins() override;
	// End of UEdGraphNode interface.

	// UK2Node interface.
	virtual bool IsNodePure() const override;
	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void PreloadRequiredAssets() override;
	virtual bool ShouldShowNodeProperties() const override;
	// End of UK2Node interface.

	// Returns the pull-down struct to construct on this node.
	UScriptStruct* GetPulldownStruct() const;

	// Sets the pull-down struct to construct on this node.
	void SetPulldownStruct(UScriptStruct* NewPulldownStruct);

	// Returns the value selected on this node.
	const FName& GetSelectedValue() const;

	// Sets the value selected on this node.
	void SetSelectedValue(const FName& InSelectedValue);

protected:
	// Returns the output pin that returns the constructed pull-down struct.
	UEdGraphPin* GetOutputPin() const;

	// Applies node-stored default values onto the intermediate MakeStruct node's sub-pins.
	// Override to additionally set PulldownSource etc.
	virtual bool ApplyDefaultsToMakeStructNode(
		FKismetCompilerContext& CompilerContext,
		const UEdGraphSchema_K2* K2Schema,
		UK2Node_MakeStruct* MakeStructNode
	) const;

	// Returns whether the specified struct should be exposed as a spawner candidate for this node class.
	// Override in derived classes to specialize on a different pull-down struct flavor (e.g. NativeLess).
	virtual bool IsTargetStruct(const UScriptStruct* Struct) const;

	// Called when creating an instance of this node for each pull-down struct.
	UBlueprintNodeSpawner* HandleOnMakeStructSpawner(const UScriptStruct* Struct) const;

protected:
	// The pull-down struct to construct on this node.
	UPROPERTY()
	UScriptStruct* PulldownStruct;

	// The value to embed as the SelectedValue of the constructed pull-down struct.
	UPROPERTY(EditAnywhere, Category = "Pulldown Struct")
	FName SelectedValue;

	// The cache of text for the title of this node.
	FNodeTextCache CachedNodeTitle;

	// The cache of text for the tooltip of this node.
	FNodeTextCache CachedNodeTooltip;

	// The cache of text for the category of this node.
	FNodeTextCache CachedNodeCategory;
};
