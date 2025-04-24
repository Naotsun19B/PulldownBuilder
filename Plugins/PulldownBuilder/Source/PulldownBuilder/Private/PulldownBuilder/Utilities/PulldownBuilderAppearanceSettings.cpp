// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/PulldownBuilderAppearanceSettings.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(PulldownBuilderAppearanceSettings)
#endif

#define LOCTEXT_NAMESPACE "PulldownBuilderAppearanceSettings"

namespace PulldownBuilder
{
	namespace AppearanceSettings
	{
		static const FName ContainerName	= TEXT("Editor");
		static const FName CategoryName		= TEXT("Plugins");
		static const FName SectionName		= TEXT("PulldownBuilderAppearanceSettings");
	}
}

FVector2D UPulldownBuilderAppearanceSettings::DefaultPanelSize = FVector2D(220.f, 300.f);

UPulldownBuilderAppearanceSettings::UPulldownBuilderAppearanceSettings()
	: PanelSize(DefaultPanelSize)
	, bIsSelectWhenDoubleClick(false)
	, bShouldInlineDisplayWhenSingleProperty(true)
	, NotificationSeverity(EPulldownBuilderNotificationSeverity::Warning)
{
}

void UPulldownBuilderAppearanceSettings::Register()
{
	PulldownBuilder::FPulldownBuilderUtils::RegisterSettings(
		PulldownBuilder::AppearanceSettings::ContainerName,
		PulldownBuilder::AppearanceSettings::CategoryName,
		PulldownBuilder::AppearanceSettings::SectionName,
		LOCTEXT("SettingName", "Pulldown Builder - Appearance"),
		LOCTEXT("SettingDescription", "Settings related to the appearance of editor extensions that display a list of pull-downs."),
		GetMutableDefault<UPulldownBuilderAppearanceSettings>()
	);
}

void UPulldownBuilderAppearanceSettings::Unregister()
{
	PulldownBuilder::FPulldownBuilderUtils::UnregisterSettings(
		PulldownBuilder::AppearanceSettings::ContainerName,
		PulldownBuilder::AppearanceSettings::CategoryName,
		PulldownBuilder::AppearanceSettings::SectionName
	);
}

const UPulldownBuilderAppearanceSettings& UPulldownBuilderAppearanceSettings::Get()
{
	const auto* Settings = GetDefault<UPulldownBuilderAppearanceSettings>();
	check(IsValid(Settings));
	return *Settings;
}

#undef LOCTEXT_NAMESPACE
