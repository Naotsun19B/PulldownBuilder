// Copyright 2021 Naotsun. All Rights Reserved.

#include "PulldownBuilderSettings.h"
#include "ISettingsModule.h"
#include "RowNameUpdaters/BlueprintUpdater.h"
#include "RowNameUpdaters/DataAssetUpdater.h"
#include "RowNameUpdaters/DataTableUpdater.h"

#define LOCTEXT_NAMESPACE "PulldownBuilder"

namespace PulldownBuilderSettingsInternal
{
	static const FName ContainerName			= TEXT("Editor");
	static const FName CategoryName				= TEXT("Plugins");
	static const FName SectionName				= TEXT("PulldownBuilderSettings");

	ISettingsModule* GetSettingsModule()
	{
		return FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	}
}

UPulldownBuilderSettings::UPulldownBuilderSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bShouldUpdateWhenSourceRowNameChanged(true)
	, ActiveRowNameUpdater({
		UBlueprintUpdater::StaticClass(),
		UDataTableUpdater::StaticClass(),
		UDataAssetUpdater::StaticClass()
	})
{	
}

void UPulldownBuilderSettings::Register()
{
	if (ISettingsModule* SettingsModule = PulldownBuilderSettingsInternal::GetSettingsModule())
	{
		SettingsModule->RegisterSettings(
			PulldownBuilderSettingsInternal::ContainerName,
			PulldownBuilderSettingsInternal::CategoryName,
			PulldownBuilderSettingsInternal::SectionName,
			LOCTEXT("SettingName", "Pulldown Builder"),
			LOCTEXT("SettingDescription", "Editor settings for Pulldown Builder"),
			GetMutableDefault<UPulldownBuilderSettings>()
		);
	}
}

void UPulldownBuilderSettings::Unregister()
{
	if (ISettingsModule* SettingsModule = PulldownBuilderSettingsInternal::GetSettingsModule())
	{
		SettingsModule->UnregisterSettings(
			PulldownBuilderSettingsInternal::ContainerName,
			PulldownBuilderSettingsInternal::CategoryName,
			PulldownBuilderSettingsInternal::SectionName
		);
	}
}

#undef LOCTEXT_NAMESPACE
