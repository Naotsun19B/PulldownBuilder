// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilder/RowNameUpdaters/RowNameUpdaterBase.h"
#include "BlueprintUpdater.generated.h"

/**
 * A class that updates the FPulldownStructBase contained in a blueprint asset.
 */
UCLASS()
class PULLDOWNBUILDER_API UBlueprintUpdater : public URowNameUpdaterBase
{
	GENERATED_BODY()

protected:
	// URowNameUpdaterBase interface.
	virtual void UpdateRowNamesInternal(
		UPulldownContents* PulldownContents,
		const FName& PreChangeName,
		const FName& PostChangeName
	) override;
	// End of URowNameUpdaterBase interface.

	// Update the default values for the pins placed on the graph.
	bool UpdateGraphPins(
		const UBlueprint* Blueprint,
		const UPulldownContents* PulldownContents,
		const FName& PreChangeName,
		const FName& PostChangeName
	);
};
