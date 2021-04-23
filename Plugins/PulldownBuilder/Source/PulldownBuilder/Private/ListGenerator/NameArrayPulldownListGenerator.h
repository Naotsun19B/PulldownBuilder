// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ListGenerator/PulldownListGeneratorBase.h"
#include "NameArrayPulldownListGenerator.generated.h"

/**
 * Generate a list to be displayed in the pull-down menu from the row name of the data table asset.
 */
UCLASS()
class UNameArrayPulldownListGenerator : public UPulldownListGeneratorBase
{
	GENERATED_BODY()

public:
	// UPulldownListGeneratorBase interface.
	virtual TArray<TSharedPtr<FString>> GetDisplayStrings() const override;
	// End of UPulldownListGeneratorBase interface.

protected:
	// Name array from which the list displayed in the pull-down menu is based.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> SourceNameArray;
};
