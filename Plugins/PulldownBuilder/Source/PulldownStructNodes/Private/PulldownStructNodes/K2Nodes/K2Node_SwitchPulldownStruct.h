// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_Switch.h"
#include "K2Node_SwitchPulldownStruct.generated.h"

class UPulldownContents;

/**
 * Switch node for the value of FPulldownStructBase::SelectedValue in the pull-down structs.
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
	virtual void AllocateDefaultPins() override;
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
	virtual void RemovePin(UEdGraphPin* TargetPin) override;
	virtual FName GetPinNameGivenIndex(int32 Index) const override;
	// End of UK2Node_Switch interface.

protected:
	// Collect the selected values to choose from before creating the case pin.
	virtual void FillSelectedValues();
	
	// Called when PulldownContents has been loaded.
	virtual void HandleOnPulldownContentsLoaded(const UPulldownContents* PulldownContents);
	
protected:
	// A pull-down struct to switch on this node.
	UPROPERTY()
	UScriptStruct* PulldownStruct;

	// A list of the current entries in the struct.
	UPROPERTY()
	TArray<FName> SelectedValues;

	// A cache of text for the title of this node.
	FNodeTextCache CachedNodeTitle;
};
