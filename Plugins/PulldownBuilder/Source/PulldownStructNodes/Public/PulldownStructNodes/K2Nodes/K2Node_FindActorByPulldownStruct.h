// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_FindActorByPulldownStruct.generated.h"

class UBlueprintNodeSpawner;

/**
 * A node class that returns the actor found from the pull-down struct constructed by the UActorNamePulldownListGenerator.
 */
UCLASS()
class PULLDOWNSTRUCTNODES_API UK2Node_FindActorByPulldownStruct : public UK2Node
{
	GENERATED_BODY()

public:
	// Defines the name of the pins contained in this node.
	static const FName WorldContextObjectPinName;
	static const FName PulldownStructPinName;
	static const FName WorldAndActorIdentifierNamePinName;
	static const FName ActorClassPinName;
	
public:
	// UEdGraphNode interface.
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetKeywords() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual void AllocateDefaultPins() override;
	// End of UEdGraphNode interface.

	// UK2Node interface.
	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void PostReconstructNode() override;
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	// End of UK2Node interface.

protected:
	// Returns the type of structure of the pin connected to PulldownStruct pin.
	UScriptStruct* GetLinkedPulldownStructType() const;

	// Changes the type of the PulldownStruct pin from the type of the connected pin.
	void RefreshPulldownStructType();

	// Changes the base class of the ActorClass pin to the actor class set to
	// the UActorNamePulldownListGenerator based on the type of the PulldownStruct pin.
	void RefreshBaseActorClass();
};
