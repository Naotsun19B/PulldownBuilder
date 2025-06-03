// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/PulldownBuilderAppearanceSettings.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(PulldownBuilderAppearanceSettings)
#endif

FVector2D UPulldownBuilderAppearanceSettings::DefaultPanelSize = FVector2D(220.f, 300.f);

UPulldownBuilderAppearanceSettings::UPulldownBuilderAppearanceSettings()
	: PanelSize(DefaultPanelSize)
	, bIsSelectWhenDoubleClick(false)
	, bShouldInlineDisplayWhenSingleProperty(true)
	, NotificationSeverity(EPulldownBuilderNotificationSeverity::Warning)
	, bIsDisplayTextDisabled(false)
	, bIsTextColoringDisabled(false)
{
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

UPulldownBuilderSettings::FSettingsInfo UPulldownBuilderAppearanceSettings::GetSettingsInfo() const
{
	return FSettingsInfo(TEXT("Appearance"));
}
