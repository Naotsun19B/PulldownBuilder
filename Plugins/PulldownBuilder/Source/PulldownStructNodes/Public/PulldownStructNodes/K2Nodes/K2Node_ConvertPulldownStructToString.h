// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "K2Node_ConvertPulldownStructToString.generated.h"

class UBlueprintNodeSpawner;

/**
 * A node class that converts FPulldownStructBase::SelectedValue in the pull-down structs to FString.
 */
UCLASS()
class PULLDOWNSTRUCTNODES_API UK2Node_ConvertPulldownStructToString : public UK2Node
{
	GENERATED_BODY()

public:
	// Defines the name of the pins contained in this node.
	static const FName PulldownStructPinName;
	static const FName NameToStringInputPinName;
	
public:
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
	// End of UK2Node interface.
	
	// Returns the pull-down struct to convert on this node.
	UScriptStruct* GetPulldownStruct() const;

	// Sets the pull-down struct to convert on this node.
	void SetPulldownStruct(UScriptStruct* NewPulldownStruct);

private:
	// Called when creating an instance of this node for each pull-down structs.
	UBlueprintNodeSpawner* HandleOnMakeStructSpawner(const UScriptStruct* Struct) const;
	
protected:
	// The pull-down struct to convert on this node.
	UPROPERTY()
	UScriptStruct* PulldownStruct;

	// The cache of text for the title of this node.
	FNodeTextCache CachedNodeTitle;

	// The cache of text for the tooltip of this node.
	FNodeTextCache CachedNodeTooltip;
	
	// The cache of text for the category of this node.
	FNodeTextCache CachedNodeCategory;
};
