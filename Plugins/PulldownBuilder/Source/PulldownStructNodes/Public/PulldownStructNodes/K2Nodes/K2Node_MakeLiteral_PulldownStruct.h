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
 * A node class that constructs a literal value of a pull-down struct.
 * Exposes a single input pin of the pull-down struct type; the existing pulldown graph-pin
 * factory automatically draws the pulldown UI on it, matching the UX of MakeLiteralInt and
 * other built-in MakeLiteral nodes.
 */
UCLASS()
class PULLDOWNSTRUCTNODES_API UK2Node_MakeLiteral_PulldownStruct : public UK2Node
{
	GENERATED_BODY()

public:
	// Constructor.
	UK2Node_MakeLiteral_PulldownStruct();

	// UObject interface.
	virtual void PostLoad() override;
	// End of UObject interface.

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
	// End of UK2Node interface.

	// Returns the pull-down struct to construct on this node.
	UScriptStruct* GetPulldownStruct() const;

	// Sets the pull-down struct to construct on this node.
	void SetPulldownStruct(UScriptStruct* NewPulldownStruct);

protected:
	// Returns the input pin that carries the literal value (drives the on-node pulldown UI).
	UEdGraphPin* GetInputPin() const;

	// Returns the output pin that returns the constructed literal.
	UEdGraphPin* GetOutputPin() const;

	// Copies values from the input pin's struct-literal default value onto the sub-pins of the
	// intermediate MakeStruct node. Override in derived classes to handle additional fields
	// (e.g. FNativeLessPulldownStruct::PulldownSource).
	virtual bool ApplyInputPinDefaultsToMakeStructNode(
		FKismetCompilerContext& CompilerContext,
		const UEdGraphSchema_K2* K2Schema,
		UK2Node_MakeStruct* MakeStructNode,
		const UEdGraphPin* InputPin
	) const;

	// Returns whether the specified struct should be exposed as a spawner candidate for this node class.
	// Override in derived classes to specialize on a different pull-down struct flavor (e.g. NativeLess).
	virtual bool IsTargetStruct(const UScriptStruct* Struct) const;

	// Returns whether there is still legacy UPROPERTY data (saved on an older version of this node)
	// that has not been migrated onto the input pin's default-value string yet.
	virtual bool HasLegacyDefaultValues() const;

	// Writes the legacy UPROPERTY data into the given pull-down struct literal string.
	// Override in derived classes to also forward additional fields (e.g. PulldownSource).
	virtual void ApplyLegacyDefaultValuesToString(FString& InOutDefaultValueString) const;

	// Clears the legacy UPROPERTY data so the migration runs only once.
	// Override in derived classes to also clear additional fields.
	virtual void ClearLegacyDefaultValues();

	// Called when creating an instance of this node for each pull-down struct.
	UBlueprintNodeSpawner* HandleOnMakeStructSpawner(const UScriptStruct* Struct) const;

private:
	// Once-per-node migration of legacy UPROPERTY data into the input pin's default-value string.
	// No-op when no legacy data is present.
	void MigrateLegacyDefaultValuesToInputPin();

protected:
	// The pull-down struct to construct on this node.
	UPROPERTY()
	UScriptStruct* PulldownStruct;

	// Legacy storage of the SelectedValue from before this node moved to an input-pin UX.
	// Read once during AllocateDefaultPins / PostLoad, then cleared. Kept as UPROPERTY so existing
	// assets still deserialize without losing data during the migration window.
	UPROPERTY()
	FName SelectedValue;

	// The cache of text for the title of this node.
	FNodeTextCache CachedNodeTitle;

	// The cache of text for the tooltip of this node.
	FNodeTextCache CachedNodeTooltip;

	// The cache of text for the category of this node.
	FNodeTextCache CachedNodeCategory;

	// Pin name for the input pin that carries the literal value.
	static const FName InputPinName;
};
