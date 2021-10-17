// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "StringTablePulldownListGenerator.generated.h"

/**
 * Generate a list to be displayed in the pull-down menu from the row name of the string table asset.
 */
UCLASS()
class PULLDOWNBUILDER_API UStringTablePulldownListGenerator : public UPulldownListGeneratorBase
{
	GENERATED_BODY()

public:
	// UPulldownListGeneratorBase interface.
	virtual TArray<TSharedPtr<FPulldownRow>> GetPulldownRows() const override;
	virtual bool HasSourceAsset() const override;
	virtual FString GetSourceAssetName() const override;
	// End of UPulldownListGeneratorBase interface.

protected:
	// The string table asset from which the list displayed in the pull-down menu is based.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pulldown")
	TSoftObjectPtr<UStringTable> SourceStringTable;
};
