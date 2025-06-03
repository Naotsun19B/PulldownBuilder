// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "PulldownBuilder/Utilities/PulldownBuilderSettings.h"
#include "PulldownBuilder/RowNameUpdaters/RowNameUpdaterBase.h"
#include "PulldownBuilderRedirectSettings.generated.h"

/**
 * An editor preferences class for redirect processing when changes are made to the data that is the source of the pull-down list.
 */
UCLASS(DefaultConfig)
class PULLDOWNBUILDER_API UPulldownBuilderRedirectSettings : public UPulldownBuilderSettings
{
	GENERATED_BODY()

public:
	// *** [Caution]: This is an experimental feature. ***
	// Whether to replace the value already used in the Blueprint Asset with the changed name
	// when the original data of the PulldownContents asset is updated
	// (for example, the row name of the data table asset is changed).
	// If this flag is off, all values already in use will be None
	// if you update the underlying data for the PulldownContents asset.
	// This process may load a large number of assets and stop the editor.
	// In that case, turn off this flag or limit the types of assets
	// that can be updated with the ActiveRowNameUpdater.
	UPROPERTY(EditAnywhere, Config, Category = "Redirect", DisplayName = "Should Update When Source Row Name Changed (Experimental)")
	bool bShouldUpdateWhenSourceRowNameChanged;

	// The list of active per-asset updater class.
	// Blueprint, data table, and data asset are provided by default.
	UPROPERTY(EditAnywhere, Config, Category = "Redirect", meta = (EditCondition = "bShouldUpdateWhenSourceRowNameChanged"))
	TArray<TSubclassOf<URowNameUpdaterBase>> ActiveRowNameUpdater;
	
public:
    // Constructor.
	UPulldownBuilderRedirectSettings();

	// UPulldownBuilderSettings interface.
	virtual FSettingsInfo GetSettingsInfo() const override;
	// End of UPulldownBuilderSettings interface.
};
