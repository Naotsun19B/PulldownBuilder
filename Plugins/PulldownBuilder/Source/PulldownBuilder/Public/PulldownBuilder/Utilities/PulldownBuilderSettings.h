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
	// A struct that information on registered settings.
	struct PULLDOWNBUILDER_API FSettingsInfo
	{
	public:
		// Constructor.
		explicit FSettingsInfo(const FName& InSectionName);
		FSettingsInfo(
			const FName& InSectionName,
			const FText& InDisplayName,
			const FText& InDescription
		);

		// Returns text with the name of the plugin at the beginning of the display name.
		FText GetFormattedDisplayName() const;
	
	public:
		// The section name to register in the editor preference.
		FName SectionName;

		// The display name in the editor preference of section name.
		FText DisplayName;

		// The tooltip text displayed in the editor preferences.
		FText Description;
	};
	
public:
	// Registers-Unregisters in the editor setting item.
	static void Register();
	static void Unregister();

	// Returns all registered editor settings classes about GraphPrinter.
	static const TArray<FSettingsInfo>& GetAllSettings();

	// Returns the section name to register in the editor preference.
	virtual FSettingsInfo GetSettingsInfo() const PURE_VIRTUAL(UPulldownBuilderSettings::GetSectionName, return FSettingsInfo(NAME_None);)
	
private:
	// Called when the end of UEngine::Init, right before loading PostEngineInit modules for both normal execution and commandlets
	static void HandleOnPostEngineInit();

private:
	// The list of all registered editor settings classes about GraphPrinter.
	static TArray<FSettingsInfo> AllSettings;
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
	static TSettings& GetMutableSettings(typename TSettings::FPassKey)
	{
		static_assert(TIsDerivedFrom<TSettings, UPulldownBuilderSettings>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of UPulldownBuilderSettings.");

		auto* Settings = GetMutableDefault<TSettings>();
		check(IsValid(Settings));
		return *Settings;
	}
}
