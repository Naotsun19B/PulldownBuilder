// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PulldownBuilderSettings.generated.h"

class URowNameUpdaterBase;

/**
 * Editor settings for this plugin.
 */
UCLASS(Config = Editor, DefaultConfig)
class UPulldownBuilderSettings : public UObject
{
	GENERATED_UCLASS_BODY()
	
public:
	// Size of the PulldownSelector widget.
	UPROPERTY(EditAnywhere, Config, Category = "Pulldown Selector")
	FVector2D PanelSize;

	// Whether you need to double-click to select an item in PulldownSelector widget.
	UPROPERTY(EditAnywhere, Config, Category = "Pulldown Selector")
	bool bIsSelectWhenDoubleClick;
	
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
	// Register - unregister in the editor setting item.
	static void Register();
	static void Unregister();
};
