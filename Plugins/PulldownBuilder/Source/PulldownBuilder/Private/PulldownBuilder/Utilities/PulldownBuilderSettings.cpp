// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/PulldownBuilderSettings.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
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
		static const FName CategoryName		= TEXT("Plugins");

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

FName UPulldownBuilderSettings::GetSectionName() const
{
	return *(PulldownBuilder::Global::PluginName.ToString() + GetSettingsName());
}

FText UPulldownBuilderSettings::GetDisplayName() const
{
	return FText::Format(
		LOCTEXT("DisplayNameFormat", "{0} - {1}"),
		FText::FromString(FName::NameToDisplayString(PulldownBuilder::Global::PluginName.ToString(), false)),
		FText::FromString(GetSettingsName())
	);
}

FText UPulldownBuilderSettings::GetTooltipText() const
{
	const UClass* SettingsClass = GetClass();
	check(IsValid(SettingsClass));
	return SettingsClass->GetToolTipText();
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
		
		SettingsModule->RegisterSettings(
			PulldownBuilder::Settings::ContainerName,
			PulldownBuilder::Settings::CategoryName,
			Settings->GetSectionName(),
			Settings->GetDisplayName(),
			Settings->GetTooltipText(),
			Settings
		);

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
		const TSharedPtr<ISettingsContainer> Container = SettingsModule->GetContainer(PulldownBuilder::Settings::ContainerName);
		check(Container.IsValid());
		const TSharedPtr<ISettingsCategory> Category = Container->GetCategory(PulldownBuilder::Settings::CategoryName);
		check(Category.IsValid());
		const TSharedPtr<ISettingsSection> Section = Category->GetSection(Settings->GetSectionName());
		check(Section.IsValid());
		Section->Save();
		
		SettingsModule->UnregisterSettings(
			PulldownBuilder::Settings::ContainerName,
			PulldownBuilder::Settings::CategoryName,
			Settings->GetSectionName()
		);

		Settings->RemoveFromRoot();
	}
}

TArray<UPulldownBuilderSettings*> UPulldownBuilderSettings::AllSettings;

#undef LOCTEXT_NAMESPACE
