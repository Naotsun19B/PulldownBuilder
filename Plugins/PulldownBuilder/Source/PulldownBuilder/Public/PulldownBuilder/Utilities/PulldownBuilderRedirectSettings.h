// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Templates/SubclassOf.h"
#include "PulldownBuilder/RowNameUpdaters/RowNameUpdaterBase.h"
#include "PulldownBuilderRedirectSettings.generated.h"

/**
 * Settings related to redirect processing when changes are made to the data that is the source of the pull-down list.
 */
UCLASS(Config = Editor, DefaultConfig)
class PULLDOWNBUILDER_API UPulldownBuilderRedirectSettings : public UObject
{
	GENERATED_BODY()

public:
	// Whether to replace the value already used in the Blueprint Asset with the changed name
	// when the original data of the PulldownContents asset is updated
	// (for example, the row name of the data table asset is changed).
	// If this flag is off, all values already in use will be None
	// if you update the underlying data for the PulldownContents asset.
	// This process may load a large number of assets and stop the editor.
	// In that case, turn off this flag or limit the types of assets
	// that can be updated with the ActiveRowNameUpdater.
	UPROPERTY(EditAnywhere, Config, Category = "Redirect")
	bool bShouldUpdateWhenSourceRowNameChanged;

	// Active per-asset updater class.
	// Blueprint, data table, and data asset are provided by default.
	UPROPERTY(EditAnywhere, Config, Category = "Redirect", meta = (EditCondition = "bShouldUpdateWhenSourceRowNameChanged"))
	TArray<TSubclassOf<URowNameUpdaterBase>> ActiveRowNameUpdater;
	
public:
    // Constructor.
	UPulldownBuilderRedirectSettings();

	// Register - unregister in the editor setting item.
	static void Register();
	static void Unregister();

	// Returns reference of this settings.
	static const UPulldownBuilderRedirectSettings& Get();
};
