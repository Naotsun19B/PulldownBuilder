// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownBuilderSettings.h"
#include "PulldownBuilder/RowNameUpdaters/BlueprintUpdater.h"
#include "PulldownBuilder/RowNameUpdaters/DataAssetUpdater.h"
#include "PulldownBuilder/RowNameUpdaters/DataTableUpdater.h"
#include "Modules/ModuleManager.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "PulldownBuilderSettings"

namespace PulldownBuilder
{
	namespace Settings
	{
		static const FName ContainerName			= TEXT("Editor");
		static const FName CategoryName				= TEXT("Plugins");
		static const FName SectionName				= TEXT("PulldownBuilderSettings");

		ISettingsModule* GetSettingsModule()
		{
			return FModuleManager::GetModulePtr<ISettingsModule>("Settings");
		}
	}
}

UPulldownBuilderSettings::UPulldownBuilderSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, PanelSize(220.f, 300.f)
	, bIsSelectWhenDoubleClick(false)
	, bShouldUpdateWhenSourceRowNameChanged(true)
{
	ActiveRowNameUpdater.Add(UBlueprintUpdater::StaticClass());
	ActiveRowNameUpdater.Add(UDataTableUpdater::StaticClass());
	ActiveRowNameUpdater.Add(UDataAssetUpdater::StaticClass());
}

void UPulldownBuilderSettings::Register()
{
	if (ISettingsModule* SettingsModule = PulldownBuilder::Settings::GetSettingsModule())
	{
		SettingsModule->RegisterSettings(
			PulldownBuilder::Settings::ContainerName,
			PulldownBuilder::Settings::CategoryName,
			PulldownBuilder::Settings::SectionName,
			LOCTEXT("SettingName", "Pulldown Builder"),
			LOCTEXT("SettingDescription", "Editor settings for Pulldown Builder"),
			GetMutableDefault<UPulldownBuilderSettings>()
		);
	}
}

void UPulldownBuilderSettings::Unregister()
{
	if (ISettingsModule* SettingsModule = PulldownBuilder::Settings::GetSettingsModule())
	{
		SettingsModule->UnregisterSettings(
			PulldownBuilder::Settings::ContainerName,
			PulldownBuilder::Settings::CategoryName,
			PulldownBuilder::Settings::SectionName
		);
	}
}

const UPulldownBuilderSettings& UPulldownBuilderSettings::Get()
{
	const auto* Settings = GetDefault<UPulldownBuilderSettings>();
	check(IsValid(Settings));
	return *Settings;
}

#undef LOCTEXT_NAMESPACE
