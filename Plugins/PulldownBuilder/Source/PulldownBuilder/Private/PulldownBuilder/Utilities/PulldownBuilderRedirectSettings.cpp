// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/PulldownBuilderRedirectSettings.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownBuilder/RowNameUpdaters/BlueprintUpdater.h"
#include "PulldownBuilder/RowNameUpdaters/DataAssetUpdater.h"
#include "PulldownBuilder/RowNameUpdaters/DataTableUpdater.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(PulldownBuilderRedirectSettings)
#endif

#define LOCTEXT_NAMESPACE "PulldownBuilderRedirectSettings"

namespace PulldownBuilder
{
	namespace RedirectSettings
	{
		static const FName ContainerName	= TEXT("Editor");
		static const FName CategoryName		= TEXT("Plugins");
		static const FName SectionName		= TEXT("PulldownBuilderRedirectSettings");
	}
}

UPulldownBuilderRedirectSettings::UPulldownBuilderRedirectSettings()
	: bShouldUpdateWhenSourceRowNameChanged(false)
{
	ActiveRowNameUpdater.Add(UBlueprintUpdater::StaticClass());
	ActiveRowNameUpdater.Add(UDataTableUpdater::StaticClass());
	ActiveRowNameUpdater.Add(UDataAssetUpdater::StaticClass());
}

void UPulldownBuilderRedirectSettings::Register()
{
	PulldownBuilder::FPulldownBuilderUtils::RegisterSettings(
		PulldownBuilder::RedirectSettings::ContainerName,
		PulldownBuilder::RedirectSettings::CategoryName,
		PulldownBuilder::RedirectSettings::SectionName,
		LOCTEXT("SettingName", "Pulldown Builder - Redirect"),
		LOCTEXT("SettingDescription", "Settings related to redirect processing when changes are made to the data that is the source of the pull-down list."),
		GetMutableDefault<UPulldownBuilderRedirectSettings>()
	);
}

void UPulldownBuilderRedirectSettings::Unregister()
{
	PulldownBuilder::FPulldownBuilderUtils::UnregisterSettings(
		PulldownBuilder::RedirectSettings::ContainerName,
		PulldownBuilder::RedirectSettings::CategoryName,
		PulldownBuilder::RedirectSettings::SectionName
	);
}

const UPulldownBuilderRedirectSettings& UPulldownBuilderRedirectSettings::Get()
{
	const auto* Settings = GetDefault<UPulldownBuilderRedirectSettings>();
	check(IsValid(Settings));
	return *Settings;
}

#undef LOCTEXT_NAMESPACE
