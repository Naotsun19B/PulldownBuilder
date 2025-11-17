// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/PulldownBuilderSettings.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "Misc/CoreDelegates.h"
#include "UObject/UObjectIterator.h"
#include "Modules/ModuleManager.h"
#include "ISettingsModule.h"
#include "ISettingsContainer.h"
#include "ISettingsCategory.h"
#include "ISettingsSection.h"

#define LOCTEXT_NAMESPACE "PulldownBuilderSettings"

namespace PulldownBuilder
{
	namespace Settings
	{
		static const FName ContainerName	= TEXT("Editor");
		static const FName CategoryName		= Global::PluginName;

		ISettingsModule* GetSettingsModule()
		{
			return FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));
		}
	}
}

void UPulldownBuilderSettings::Register()
{
	FCoreDelegates::OnPostEngineInit.AddStatic(&UPulldownBuilderSettings::HandleOnPostEngineInit);
	FCoreDelegates::OnEnginePreExit.AddStatic(&UPulldownBuilderSettings::HandleOnEnginePreExit);
}

bool UPulldownBuilderSettings::ShouldRegisterToSettingsPanel() const
{
	return true;
}

FName UPulldownBuilderSettings::GetSectionName() const
{
	return *(PulldownBuilder::Global::PluginName.ToString() + GetSettingsName());
}

FText UPulldownBuilderSettings::GetDisplayName() const
{
	return FText::FromString(FName::NameToDisplayString(*GetSettingsName(), false));
}

FText UPulldownBuilderSettings::GetTooltipText() const
{
	const UClass* SettingsClass = GetClass();
	check(IsValid(SettingsClass));
	return SettingsClass->GetToolTipText();
}

void UPulldownBuilderSettings::OpenSettings(const FName SectionName)
{
	if (ISettingsModule* SettingsModule = PulldownBuilder::Settings::GetSettingsModule())
	{
		SettingsModule->ShowViewer(
			PulldownBuilder::Settings::ContainerName,
			PulldownBuilder::Settings::CategoryName,
			SectionName
		);
	}
}

const TArray<UPulldownBuilderSettings*>& UPulldownBuilderSettings::GetAllSettings()
{
	return AllSettings;
}

void UPulldownBuilderSettings::HandleOnPostEngineInit()
{
	ISettingsModule* SettingsModule = PulldownBuilder::Settings::GetSettingsModule();
	if (SettingsModule == nullptr)
	{
		return;
	}
	
	for (auto* Settings : TObjectRange<UPulldownBuilderSettings>(RF_NoFlags))
	{
		if (!IsValid(Settings))
		{
			continue;
		}

		const UClass* SettingsClass = Settings->GetClass();
		if (!IsValid(SettingsClass))
		{
			continue;
		}
		if (SettingsClass->HasAnyClassFlags(CLASS_Abstract))
		{
			continue;
		}
		if (!Settings->IsTemplate())
		{
			continue;
		}

		if (Settings->ShouldRegisterToSettingsPanel())
		{
			SettingsModule->RegisterSettings(
				PulldownBuilder::Settings::ContainerName,
				PulldownBuilder::Settings::CategoryName,
				Settings->GetSectionName(),
				Settings->GetDisplayName(),
				Settings->GetTooltipText(),
				Settings
			);
		}
		
		Settings->AddToRoot();
		AllSettings.Add(Settings);
	}
}

void UPulldownBuilderSettings::HandleOnEnginePreExit()
{
	ISettingsModule* SettingsModule = PulldownBuilder::Settings::GetSettingsModule();
	if (SettingsModule == nullptr)
	{
		return;
	}

	for (auto* Settings : AllSettings)
	{
		if (Settings->ShouldRegisterToSettingsPanel())
		{
			SettingsModule->UnregisterSettings(
				PulldownBuilder::Settings::ContainerName,
				PulldownBuilder::Settings::CategoryName,
				Settings->GetSectionName()
			);
		}
		
		Settings->PreSaveConfig();
		
		const UClass* SettingsClass = Settings->GetClass();
		check(IsValid(SettingsClass));
		if (SettingsClass->HasAnyClassFlags(CLASS_DefaultConfig))
		{
#if UE_5_00_OR_LATER
			Settings->TryUpdateDefaultConfigFile();
#else
			Settings->UpdateDefaultConfigFile();
#endif
		}
		else if (SettingsClass->HasAnyClassFlags(CLASS_GlobalUserConfig))
		{
			Settings->UpdateGlobalUserConfigFile();
		}
		else if (SettingsClass->HasAnyClassFlags(CLASS_ProjectUserConfig))
		{
			Settings->UpdateProjectUserConfigFile();
		}
		else
		{
			const FString& ConfigFilename = GetConfigFilename(Settings);
#if UE_5_00_OR_LATER
			Settings->TryUpdateDefaultConfigFile(ConfigFilename);
#else
			Settings->UpdateDefaultConfigFile(ConfigFilename);
#endif
		}
		
		Settings->RemoveFromRoot();
	}
}

TArray<UPulldownBuilderSettings*> UPulldownBuilderSettings::AllSettings;

#undef LOCTEXT_NAMESPACE
