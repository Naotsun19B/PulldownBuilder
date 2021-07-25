// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PulldownListGeneratorBase.generated.h"

/**
 * A generator class that generates a list to display in a pull-down menu.
 */
UCLASS(Abstract, Blueprintable, EditInlineNew)
class PULLDOWNBUILDER_API UPulldownListGeneratorBase : public UObject
{
	GENERATED_BODY()

public:
	// Returns a list of strings to display in the pull-down menu.
	// By default, it returns the value of "GetDisplayStringsBP".
	virtual TArray<TSharedPtr<FString>> GetDisplayStrings() const;

protected:
	UFUNCTION(BlueprintPure, BlueprintImplementableEvent, Category = "Pulldown", meta = (BlueprintProtected, DisplayName = "GetDisplayStrings"))
    TArray<FString> GetDisplayStringsFromBlueprint() const;

	// Update all FPulldownStructBases that reference owner PulldownContents asset.
	virtual void UpdateDisplayStrings(const FName& PreChangeName, const FName& PostChangeName);
};
