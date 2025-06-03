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

UPulldownBuilderSettings::FSettingsInfo::FSettingsInfo(const FName& InSectionName)
	: SectionName(InSectionName)
{
	DisplayName = FText::FromString(FName::NameToDisplayString(SectionName.ToString(), false));
}

UPulldownBuilderSettings::FSettingsInfo::FSettingsInfo(const FName& InSectionName, const FText& InDisplayName, const FText& InDescription)
	: SectionName(InSectionName)
	, DisplayName(InDisplayName)
	, Description(InDescription)
{
}

FText UPulldownBuilderSettings::FSettingsInfo::GetFormattedDisplayName() const
{
	return FText::Format(
		LOCTEXT("DisplayNameFormat", "{0} - {1}"),
		FText::FromString(FName::NameToDisplayString(PulldownBuilder::Global::PluginName.ToString(), false)),
		DisplayName
	);
}

void UPulldownBuilderSettings::Register()
{
	FCoreDelegates::OnPostEngineInit.AddStatic(&UPulldownBuilderSettings::HandleOnPostEngineInit);
}

void UPulldownBuilderSettings::Unregister()
{
	ISettingsModule* SettingsModule = PulldownBuilder::Settings::GetSettingsModule();
	if (SettingsModule == nullptr)
	{
		return;
	}

	for (const auto& Settings : AllSettings)
	{
		const TSharedPtr<ISettingsContainer> Container = SettingsModule->GetContainer(PulldownBuilder::Settings::ContainerName);
		check(Container.IsValid());
		const TSharedPtr<ISettingsCategory> Category = Container->GetCategory(PulldownBuilder::Settings::CategoryName);
		check(Category.IsValid());
		const TSharedPtr<ISettingsSection> Section = Category->GetSection(Settings.SectionName);
		check(Section.IsValid());
		Section->Save();
		
		SettingsModule->UnregisterSettings(
			PulldownBuilder::Settings::ContainerName,
			PulldownBuilder::Settings::CategoryName,
			Settings.SectionName
		);
	}
}

const TArray<UPulldownBuilderSettings::FSettingsInfo>& UPulldownBuilderSettings::GetAllSettings()
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
	
	for (auto* GraphPrinterSettings : TObjectRange<UPulldownBuilderSettings>(RF_NoFlags))
	{
		if (!IsValid(GraphPrinterSettings))
		{
			continue;
		}

		const UClass* SettingsClass = GraphPrinterSettings->GetClass();
		if (!IsValid(SettingsClass))
		{
			continue;
		}
		if (SettingsClass->HasAnyClassFlags(CLASS_Abstract))
		{
			continue;
		}

		FSettingsInfo SettingsInfo = GraphPrinterSettings->GetSettingsInfo();
		if (SettingsInfo.Description.IsEmpty())
		{
			SettingsInfo.Description = SettingsClass->GetToolTipText();
		}
		
		SettingsModule->RegisterSettings(
			PulldownBuilder::Settings::ContainerName,
			PulldownBuilder::Settings::CategoryName,
			SettingsInfo.SectionName,
			SettingsInfo.GetFormattedDisplayName(),
			SettingsInfo.Description,
			GraphPrinterSettings
		);

		AllSettings.Add(SettingsInfo);
	}
}

TArray<UPulldownBuilderSettings::FSettingsInfo> UPulldownBuilderSettings::AllSettings;

#undef LOCTEXT_NAMESPACE
