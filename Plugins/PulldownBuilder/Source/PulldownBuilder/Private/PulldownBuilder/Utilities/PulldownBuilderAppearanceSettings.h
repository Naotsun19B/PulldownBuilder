// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PulldownBuilderAppearanceSettings.generated.h"

/**
 * Settings related to the appearance of editor extensions that display a list of pull-downs.
 */
UCLASS(Config = Editor)
class UPulldownBuilderAppearanceSettings : public UObject
{
	GENERATED_BODY()
	
public:
	// Size of the PulldownSelector widget.
	UPROPERTY(EditAnywhere, Config, Category = "Pulldown Selector")
	FVector2D PanelSize;

	// Whether you need to double-click to select an item in PulldownSelector widget.
	UPROPERTY(EditAnywhere, Config, Category = "Pulldown Selector")
	bool bIsSelectWhenDoubleClick;

public:
	// Constructor.
	UPulldownBuilderAppearanceSettings();

	// Register - unregister in the editor setting item.
	static void Register();
	static void Unregister();

	// Returns reference of this settings.
	static const UPulldownBuilderAppearanceSettings& Get();
};
