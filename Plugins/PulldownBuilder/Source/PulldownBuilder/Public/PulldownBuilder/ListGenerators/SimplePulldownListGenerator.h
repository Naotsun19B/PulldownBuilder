// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "SimplePulldownListGenerator.generated.h"

/**
 * A generator class that generates a list to be displayed in the pull-down menu from the list of FPulldownRows set directly by the user.
 */
UCLASS()
class PULLDOWNBUILDER_API USimplePulldownListGenerator : public UPulldownListGeneratorBase
{
	GENERATED_BODY()

public:
	// UObject interface.
#if UE_4_25_OR_LATER
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
#else
	virtual void PreEditChange(UProperty* PropertyAboutToChange) override;
#endif
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface.
	
	// UPulldownListGeneratorBase interface.
	virtual FPulldownRows GetPulldownRows(
		const TArray<UObject*>& OuterObjects,
		const FStructContainer& StructInstance
	) const override;
	// End of UPulldownListGeneratorBase interface.

protected:
	// Expansion points for implementation in blueprints.
	UFUNCTION(BlueprintImplementableEvent, Category = "Pulldown", meta = (Tooltip = "Called before a source name array is changed."))
	void PrePulldownRowsModify();
	UFUNCTION(BlueprintImplementableEvent, Category = "Pulldown", meta = (Tooltip = "Called after a source name array has been changed."))
	void PostPulldownRowsModify();

	// UPulldownListGeneratorBase interface.
	virtual TArray<FName> GetDefaultValueOptions_Implementation() const override;
	// End of UPulldownListGeneratorBase interface.
	
protected:
	// The list of pull-down lines that appear in the list in the pull-down menu.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pulldown")
	mutable TArray<FPulldownRow> PulldownRows;

	// The cache of selected values before change.
	UPROPERTY(Transient)
	TArray<FName> PreChangeSelectedValues;
};
