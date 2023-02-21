// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownStructNodes/K2Nodes/K2Node_SwitchPulldownStruct.h"
#include "K2Node_SwitchNativeLessPulldownStruct.generated.h"

class UPulldownContents;

/**
 * 
 */
UCLASS(HideCategories = "PinOptions")
class PULLDOWNSTRUCTNODES_API UK2Node_SwitchNativeLessPulldownStruct : public UK2Node_SwitchPulldownStruct
{
	GENERATED_BODY()

public:
	// Constructor.
	UK2Node_SwitchNativeLessPulldownStruct();

	// UObject interface.
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void Serialize(FArchive& Ar) override;
	// End of UObject interface.
	
	// UEdGraphNode interface.
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	// End of UEdGraphNode interface.
	
	// UK2Node interface.
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual bool ShouldShowNodeProperties() const override;
	// End of UK2Node interface.

	// UK2Node_SwitchPulldownStruct interface.
	virtual void FillSelectedValues() override;
	// End of UK2Node_SwitchPulldownStruct interface.

protected:
	//
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UPulldownContents> PulldownContents;
};
