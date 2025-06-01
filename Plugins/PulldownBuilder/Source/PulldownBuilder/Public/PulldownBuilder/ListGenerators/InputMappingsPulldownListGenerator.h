// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "InputMappingsPulldownListGenerator.generated.h"

/**
 * A generator class that generates a list to be displayed in the pull-down menu from the input mappings set in the project settings.
 */
UCLASS()
class PULLDOWNBUILDER_API UInputMappingsPulldownListGenerator : public UPulldownListGeneratorBase
{
	GENERATED_BODY()

public:
	// Constructor.
	UInputMappingsPulldownListGenerator();

	// UObject interface.
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void BeginDestroy() override;
	// End of UObject interface.
	
	// UPulldownListGeneratorBase interface.
	virtual FPulldownRows GetPulldownRows(
		const TArray<UObject*>& OuterObjects,
		const FStructContainer& StructInstance
	) const override;
	// End of UPulldownListGeneratorBase interface.

protected:
	// Gets a list of input information from UInputSettings.
	FPulldownRows GetPulldownRowsFromInputSettings() const;
	
	// Caches a pull-down list of current input mapping.
	void CachePreChangeDisplayTexts();
	
	// Called when action or axis mappings have been changed.
	void HandleOnActionAxisMappingsChanged();

	// Expansion point for implementation in blueprints.
	UFUNCTION(BlueprintImplementableEvent, Category = "Pulldown", meta = (Tooltip = "Called when action or axis mappings have been changed."))
	void OnActionAxisMappingsChanged();

	// UPulldownListGeneratorBase interface.
	virtual TArray<FName> GetDefaultValueOptions_Implementation() const override;
	// End of UPulldownListGeneratorBase interface.
	
protected:
	// Whether to include the action mapping in the list in thepull-down menu.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pulldown")
	bool bIncludeActionMappings;

	// Whether to include the axis mapping in the list in the pull-down menu.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pulldown")
	bool bIncludeAxisMappings;

	// Whether to include the speech mapping in the list in the pull-down menu.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pulldown")
	bool bIncludeSpeechMappings;

	// The cache of the pull-down list before the change for the redirect process.
	UPROPERTY(Transient)
	TArray<FName> PreChangeRowNames;
};
