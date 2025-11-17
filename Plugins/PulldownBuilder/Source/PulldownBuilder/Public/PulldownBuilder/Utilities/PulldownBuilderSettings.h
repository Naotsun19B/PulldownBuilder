// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PulldownBuilder/Utilities/PassKey.h"
#include "PulldownBuilderSettings.generated.h"

/**
 * An editor preferences class for this plugin that automatically registers-unregisters.
 */
UCLASS(Abstract, Config = Editor)
class PULLDOWNBUILDER_API UPulldownBuilderSettings : public UObject
{
	GENERATED_BODY()
	
public:
	// Registers in the editor setting item.
	static void Register();

	// Returns whether to register it to the editor's settings panel and display it on the editor.
	virtual bool ShouldRegisterToSettingsPanel() const;
	
	// Returns the name of the section registered in ISettingsModule.
	virtual FName GetSectionName() const;

	// Returns the name of the settings that will be displayed in the editor.
	virtual FText GetDisplayName() const;

	// Returns tooltip text for settings displayed in the editor.
	virtual FText GetTooltipText() const;

	// Opens the settings menu for this plugin.
	static void OpenSettings(const FName SectionName);

	// Returns the list of all registered editor settings classes about this plugin.
	static const TArray<UPulldownBuilderSettings*>& GetAllSettings();

protected:
	// Returns the unique name of this setting.
	virtual FString GetSettingsName() const PURE_VIRTUAL(UPulldownBuilderSettings::GetSettingsName, return {};)

	// Called before saving to the config file.
	virtual void PreSaveConfig() {}
	
private:
	// Called when the end of UEngine::Init, right before loading PostEngineInit modules for both normal execution and commandlets.
	static void HandleOnPostEngineInit();

	// Called before the engine exits. Separate from OnPreExit as OnEnginePreExit occurs before shutting down any core modules.
	static void HandleOnEnginePreExit();

private:
	// The list of all registered editor settings classes about this plugin.
	static TArray<UPulldownBuilderSettings*> AllSettings;
};

namespace PulldownBuilder
{
	// Returns reference of settings.
	template<class TSettings>
	static const TSettings& GetSettings()
	{
		static_assert(TIsDerivedFrom<TSettings, UPulldownBuilderSettings>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of UPulldownBuilderSettings.");

		const auto* Settings = GetDefault<TSettings>();
		check(IsValid(Settings));
		return *Settings;
	}

	// Returns mutable reference of settings.
	template<class TSettings>
	static TSettings& GetMutableSettings(
#if UE_5_00_OR_LATER
		typename TSettings::FPassKey
#endif
	)
	{
		static_assert(TIsDerivedFrom<TSettings, UPulldownBuilderSettings>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of UPulldownBuilderSettings.");

		auto* Settings = GetMutableDefault<TSettings>();
		check(IsValid(Settings));
		return *Settings;
	}
}
