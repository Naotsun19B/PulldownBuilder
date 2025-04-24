// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownStructNodes/K2Nodes/K2Node_SwitchPulldownStruct.h"
#include "K2Node_SwitchNativeLessPulldownStruct.generated.h"

#if WITH_EDITOR
class UPulldownContents;
#endif

/**
 * A switch node for the value of FNativelessPulldownStructBase::SelectedValue in the pull-down structs.
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

#if WITH_EDITOR
	// Returns the PulldownContents asset from which this switch node's pins originate.
	TSoftObjectPtr<UPulldownContents> GetPulldownContents() const;

	// Sets the PulldownContents asset from which this switch node's pins originate.
	void SetPulldownContents(const TSoftObjectPtr<UPulldownContents>& NewPulldownContents);
#endif
	
protected:
#if WITH_EDITORONLY_DATA
	// The PulldownContents asset from which this switch node's pins originate.
	UPROPERTY(EditAnywhere, Category = "Native Less Pulldown Struct")
	TSoftObjectPtr<UPulldownContents> PulldownContents;
#endif
};
