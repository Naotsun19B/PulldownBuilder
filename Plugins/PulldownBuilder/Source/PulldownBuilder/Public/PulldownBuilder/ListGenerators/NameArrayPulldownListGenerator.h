// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "NameArrayPulldownListGenerator.generated.h"

/**
 * A generator class that generates a list to be displayed in the pull-down menu from the name array.
 */
UCLASS()
class PULLDOWNBUILDER_API UNameArrayPulldownListGenerator : public UPulldownListGeneratorBase
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
	virtual TArray<TSharedPtr<FPulldownRow>> GetPulldownRows(
		const TArray<UObject*>& OuterObjects,
		const FStructContainer& StructInstance
	) const override;
	// End of UPulldownListGeneratorBase interface.

protected:
	// Expansion points for implementation in blueprints.
	UFUNCTION(BlueprintImplementableEvent, Category = "Pulldown", meta = (Tooltip = "Called before a source name array is changed."))
	void PreSourceNameArrayModify();
	UFUNCTION(BlueprintImplementableEvent, Category = "Pulldown", meta = (Tooltip = "Called after a source name array has been changed."))
	void PostSourceNameArrayModify();
	
protected:
	// The map of name and tooltip text pair that is the basis of the list that appears in the pull-down menu.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pulldown")
	mutable TMap<FName, FName> SourceNameArray;

	// The cache of row list before change.
	UPROPERTY(Transient)
	TArray<FName> PreChangeRowNames;
};
