﻿// Copyright 2021-2022 Naotsun. All Rights Reserved.

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
	
	// UPulldownListGeneratorBase interface.
	virtual TArray<TSharedPtr<FPulldownRow>> GetPulldownRows() const override;
	// End of UPulldownListGeneratorBase interface.

protected:
	// Whether to include the action mapping in the list in the pulldown menu.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pulldown")
	bool bIncludeActionMappings;

	// Whether to include the axis mapping in the list in the pulldown menu.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pulldown")
	bool bIncludeAxisMappings;

	// Whether to include the speech mapping in the list in the pulldown menu.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pulldown")
	bool bIncludeSpeechMappings;
};
