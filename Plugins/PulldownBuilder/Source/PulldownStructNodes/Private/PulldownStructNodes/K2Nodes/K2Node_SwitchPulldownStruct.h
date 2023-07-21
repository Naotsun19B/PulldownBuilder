// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_Switch.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "K2Node_SwitchPulldownStruct.generated.h"

class UPulldownContents;
class UBlueprintNodeSpawner;

/**
 * A switch node for the value of FPulldownStructBase::SelectedValue in the pull-down structs.
 */
UCLASS()
class PULLDOWNSTRUCTNODES_API UK2Node_SwitchPulldownStruct : public UK2Node_Switch
{
	GENERATED_BODY()

public:
	// Constructor.
	UK2Node_SwitchPulldownStruct();

	// UObject interface.
	virtual void PostLoad() override;
	virtual void BeginDestroy() override;
	virtual void Serialize(FArchive& Ar) override;
	// End of UObject interface.
	
	// UEdGraphNode interface.
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	// End of UEdGraphNode interface.

	// UK2Node interface.
	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual ERedirectType DoPinsMatchForReconstruction(
		const UEdGraphPin* NewPin,
		int32 NewPinIndex,
		const UEdGraphPin* OldPin,
		int32 OldPinIndex
	) const override;
	virtual FNodeHandlingFunctor* CreateNodeHandler(FKismetCompilerContext& CompilerContext) const override;
	virtual bool CanEverRemoveExecutionPin() const override;
	virtual bool CanUserEditPinAdvancedViewFlag() const override;
	virtual void PreloadRequiredAssets() override;
	// End of UK2Node interface.

	// UK2Node_Switch interface.
	virtual FEdGraphPinType GetPinType() const override;
	virtual void AddPinToSwitchNode() override;
	virtual void RemovePinFromSwitchNode(UEdGraphPin* TargetPin) override;
	virtual void CreateSelectionPin() override;
	virtual void CreateCasePins() override;
	virtual void CreateFunctionPin() override;
	virtual void RemovePin(UEdGraphPin* TargetPin) override;
	virtual FName GetPinNameGivenIndex(int32 Index) const override;
	// End of UK2Node_Switch interface.

protected:
	// Collects the selected values to choose from before creating the case pin.
	virtual void FillSelectedValues();
	
	// Called when PulldownContents has been loaded.
	virtual void HandleOnPulldownContentsLoaded(const UPulldownContents* LoadedPulldownContents);

	// Called when a value added to the pull-down menu.
	virtual void HandleOnPulldownRowAdded(UPulldownContents* ModifiedPulldownContents, const FName& AddedRowName);

	// Called when a value removed from the pull-down menu.
	virtual void HandleOnPulldownRowRemoved(UPulldownContents* ModifiedPulldownContents, const FName& RemovedRowName);

	// Called when a value contained in the pull-down menu has been renamed.
	virtual void HandleOnPulldownRowRenamed(
		UPulldownContents* ModifiedPulldownContents,
		const FName& PreChangeName,
		const FName& PostChangeName
	);

	// Called when the underlying data for the pull-down menu has changed.
	virtual void HandleOnPulldownContentsSourceChanged(UPulldownContents* ModifiedPulldownContents);

	// Returns whether the passed UPulldownContents is related to itself and node is required reconstruct.
	bool NeedToReconstructNode(const UPulldownContents* PulldownContents) const;
	
private:
	// Called when creating an instance of this node for each pull-down structs.
	UBlueprintNodeSpawner* HandleOnMakeStructSpawner(const UScriptStruct* Struct) const;
	
protected:
	// The pull-down struct to switch on this node.
	UPROPERTY()
	UScriptStruct* PulldownStruct;

	// The list of the current entries in the struct.
	UPROPERTY()
	TArray<FName> SelectedValues;

	// The list of display texts that override the SelectedValues string on the pull-down menu.
	TArray<FText> DisplayTexts;

	// The cache of text for the title of this node.
	FNodeTextCache CachedNodeTitle;

	// The cache of text for the tooltip of this node.
	FNodeTextCache CachedNodeTooltip;
	
	// The cache of text for the category of this node.
	FNodeTextCache CachedNodeCategory;
};
