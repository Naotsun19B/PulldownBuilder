// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PulldownBuilder/Types/PulldownBuilderNotificationSeverity.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "PulldownBuilderAppearanceSettings.generated.h"

/**
 * A editor preferences class for the appearance of editor extensions that display a list of pull-downs.
 */
UCLASS(Config = Editor, GlobalUserConfig)
class UPulldownBuilderAppearanceSettings : public UObject
{
	GENERATED_BODY()

public:
	// Define a default value for the size of the PulldownSelector widget so that it can be used externally.
	static FVector2D DefaultPanelSize;
	
public:
	// The size of the PulldownSelector widget.
	UPROPERTY(EditAnywhere, Config, Category = "Selector")
	FVector2D PanelSize;

	// Whether you need to double-click to select an item in PulldownSelector widget.
	UPROPERTY(EditAnywhere, Config, Category = "Selector")
	bool bIsSelectWhenDoubleClick;

	// Whether to automatically inline a single property pull-down struct.
	UPROPERTY(EditAnywhere, Config, Category = "Details")
	bool bShouldInlineDisplayWhenSingleProperty;

	// The severity of the notification.
	UPROPERTY(EditAnywhere, Config, Category = "Notification")
	EPulldownBuilderNotificationSeverity NotificationSeverity;

	// Whether the selected value is displayed even if it is displayed in items in the pull-down menu.
	UPROPERTY(Config)
	bool bIsDisplayTextDisabled;

	// Whether to disable text coloring in the pull-down menu and display text for all items with the default color.
	UPROPERTY(Config)
	bool bIsTextColoringDisabled;

public:
	// Constructor.
	UPulldownBuilderAppearanceSettings();

	// Registers-Unregisters in the editor setting item.
	static void Register();
	static void Unregister();
	
	// UObject interface.
	virtual void PostInitProperties() override;
#if UE_4_25_OR_LATER
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#else
	virtual bool CanEditChange(const UProperty* InProperty) const override;
#endif
	// End of UObject interface.

	// Returns reference of this settings.
	static const UPulldownBuilderAppearanceSettings& Get();
};
