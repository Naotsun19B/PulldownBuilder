// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilderTestEditor/PulldownBuilderTestEditorSettings.h"
#include "Modules/ModuleManager.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "PulldownBuilderTestEditorSettings"

namespace PulldownBuilderTest
{
	namespace Settings
	{
		static const FName ContainerName    = TEXT("Editor");
		static const FName CategoryName	    = TEXT("General");
		static const FName SectionName      = TEXT("Pulldown Builder Test");

		ISettingsModule* GetSettingsModule()
		{
			return FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));
		}
	}
}

void UPulldownBuilderTestEditorSettings::Register()
{
	if (ISettingsModule* SettingsModule = PulldownBuilderTest::Settings::GetSettingsModule())
	{
		SettingsModule->RegisterSettings(
			PulldownBuilderTest::Settings::ContainerName,
			PulldownBuilderTest::Settings::CategoryName,
			PulldownBuilderTest::Settings::SectionName,
			LOCTEXT("SettingName", "Pulldown Builder Test"),
			LOCTEXT("SettingDescription", "Editor settings for Pulldown Builder Test"),
			GetMutableDefault<UPulldownBuilderTestEditorSettings>()
		);
	}
}

void UPulldownBuilderTestEditorSettings::Unregister()
{
	if (ISettingsModule* SettingsModule = PulldownBuilderTest::Settings::GetSettingsModule())
	{
		SettingsModule->UnregisterSettings(
			PulldownBuilderTest::Settings::ContainerName,
			PulldownBuilderTest::Settings::CategoryName,
			PulldownBuilderTest::Settings::SectionName
		);
	}
}
#undef LOCTEXT_NAMESPACE
