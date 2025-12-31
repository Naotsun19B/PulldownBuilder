// Copyright 2021-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilder/RowNameUpdaters/RowNameUpdaterBase.h"
#include "DataAssetUpdater.generated.h"

/**
 * A class that updates the FPulldownStructBase contained in a data asset.
 */
UCLASS()
class PULLDOWNBUILDER_API UDataAssetUpdater : public URowNameUpdaterBase
{
	GENERATED_BODY()

protected:
	// URowNameUpdaterBase interface.
	virtual void UpdateRowNamesInternal(
		UPulldownContents* PulldownContents,
		const FName& PreChangeSelectedValue,
		const FName& PostChangeSelectedValue
	) override;
	// End of URowNameUpdaterBase interface.
};
