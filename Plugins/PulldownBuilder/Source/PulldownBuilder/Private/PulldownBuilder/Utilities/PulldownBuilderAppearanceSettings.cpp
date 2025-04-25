// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/PulldownBuilderAppearanceSettings.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"

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

void UPulldownBuilderAppearanceSettings::PostInitProperties()
{
	UObject::PostInitProperties();

#if !UE_5_00_OR_LATER
	bShouldInlineDisplayWhenSingleProperty = false;

#if UE_4_25_OR_LATER
	for (FBoolProperty* BoolProperty : TFieldRange<FBoolProperty>(StaticClass()))
#else
	for (UBoolProperty* BoolProperty : TFieldRange<UBoolProperty>(StaticClass()))
#endif
	{
#if UE_4_25_OR_LATER
		if (BoolProperty == nullptr)
#else
		if (!IsValid(BoolProperty))
#endif
		{
			continue;
		}

		if (BoolProperty->GetFName() != GET_MEMBER_NAME_CHECKED(UPulldownBuilderAppearanceSettings, bShouldInlineDisplayWhenSingleProperty))
		{
			continue;
		}

		BoolProperty->SetMetaData(
			TEXT("ToolTip"),
			TEXT("In UE4.27 and earlier, the reset to default value function cannot be implemented when inline displaying, so this setting will be disabled.")
		);
		
		break;
	}
#endif
}

#if UE_4_25_OR_LATER
bool UPulldownBuilderAppearanceSettings::CanEditChange(const FProperty* InProperty) const
#else
bool UPulldownBuilderAppearanceSettings::CanEditChange(const UProperty* InProperty) const
#endif
{
	bool bCanEditChange = true;
#if !UE_5_00_OR_LATER
#if UE_4_25_OR_LATER
	if (InProperty != nullptr)
#else
	if (IsValid(InProperty))
#endif
	{
		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UPulldownBuilderAppearanceSettings, bShouldInlineDisplayWhenSingleProperty))
		{
			bCanEditChange = false;
		}
	}
#endif
	
	return (UObject::CanEditChange(InProperty) && bCanEditChange);
}

const UPulldownBuilderAppearanceSettings& UPulldownBuilderAppearanceSettings::Get()
{
	const auto* Settings = GetDefault<UPulldownBuilderAppearanceSettings>();
	check(IsValid(Settings));
	return *Settings;
}

#undef LOCTEXT_NAMESPACE
