// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "InputMappingsPulldownListGenerator.generated.h"

/**
 * Generate a list to be displayed in the pull-down menu from the input mappings set in the project settings.
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
	virtual void BeginDestroy() override;
	// End of UObject interface.
	
	// UPulldownListGeneratorBase interface.
	virtual TArray<TSharedPtr<FPulldownRow>> GetPulldownRows(
		const TArray<UObject*>& OuterObjects,
		const FStructContainer& StructInstance
	) const override;
	// End of UPulldownListGeneratorBase interface.

protected:
	// Get a list of input information from UInputSettings.
	TArray<TSharedPtr<FPulldownRow>> GetPulldownRowsFromInputSettings() const;
	
	// Cache a pull-down list of current input mapping.
	void CachePreChangeDisplayTexts();
	
	// Called when action or axis mappings have been changed.
	void HandleOnActionAxisMappingsChanged();
	UFUNCTION(BlueprintImplementableEvent, Category = "Pulldown")
	void OnActionAxisMappingsChanged();
	
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

	// Keep the data of the pull-down list before the change for the redirect process.
	UPROPERTY(Transient)
	TArray<FName> PreChangeDisplayTexts;
};
