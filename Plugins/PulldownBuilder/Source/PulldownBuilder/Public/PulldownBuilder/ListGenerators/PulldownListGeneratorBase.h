// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PulldownBuilder/Types/PulldownRow.h"
#include "PulldownBuilder/Types/StructContainer.h"
#include "PulldownListGeneratorBase.generated.h"

/**
 * A generator class that generates a list to display in a pull-down menu.
 */
UCLASS(Abstract, Blueprintable, EditInlineNew)
class PULLDOWNBUILDER_API UPulldownListGeneratorBase : public UObject
{
	GENERATED_BODY()

public:
	// Returns a list of data to display in the pull-down menu.
	// By default, it returns the value of "GetPulldownRowsFromBlueprint".
	virtual TArray<TSharedPtr<FPulldownRow>> GetPulldownRows(
		const TArray<UObject*>& OuterObjects,
		const FStructContainer& StructInstance
	) const;

	// Returns whether there is an underlying asset.
	// If this function returns true, the name of the original asset
	// will be displayed in the PulldownContents pop-up display.
	virtual bool HasSourceAsset() const;

	// Returns the name of the underlying asset only if "HasSourceAsset" returns true.
	virtual FString GetSourceAssetName() const;
	
protected:
	UFUNCTION(BlueprintPure, BlueprintImplementableEvent, Category = "Pulldown", meta = (BlueprintProtected, DisplayName = "GetPulldownRows"))
    TArray<FPulldownRow> GetPulldownRowsFromBlueprint(
    	const TArray<UObject*>& OuterObjects,
    	const FStructContainer& StructInstance
    ) const;

	// Update all FPulldownStructBases that reference owner PulldownContents asset.
	virtual void UpdateDisplayStrings(const FName& PreChangeName, const FName& PostChangeName);
};
