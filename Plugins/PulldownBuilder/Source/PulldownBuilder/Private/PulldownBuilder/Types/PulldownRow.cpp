// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Types/PulldownRow.h"
#include "PulldownBuilder/Types/PulldownRowColors.h"
#include "PulldownBuilder/Utilities/PulldownBuilderAppearanceSettings.h"

#define LOCTEXT_NAMESPACE "PulldownRow"

const FString FPulldownRow::NoneString = LexToString(NAME_None);

FPulldownRow::FPulldownRow()
	: FPulldownRow(NoneString)
{
}

FPulldownRow::FPulldownRow(const FString& InSelectedValue, const FText& InTooltipText, const FText& InDisplayText)
	: SelectedValue(InSelectedValue)
	, DisplayText(InDisplayText)
	, TooltipText(InTooltipText)
	, bIsDefaultValue(false)
	, DisplayTextColor(PulldownBuilder::FPulldownRowColors::Normal.GetSpecifiedColor())
	, bIsNonExistentValue(false)
{
}

bool FPulldownRow::IsNone() const
{
	return (SelectedValue == NoneString);
}

FText FPulldownRow::GetDisplayText() const
{
	return (DisplayText.IsEmpty() ? FText::FromString(SelectedValue) : DisplayText);
}

bool FPulldownRow::IsNonExistentValue() const
{
	return bIsNonExistentValue;
}

FSlateColor FPulldownRow::GetDisplayTextColor() const
{
	const auto& Settings = PulldownBuilder::GetSettings<UPulldownBuilderAppearanceSettings>();
	if (Settings.bIsTextColoringDisabled)
	{
		return PulldownBuilder::FPulldownRowColors::Normal;
	}
		
	if (IsNonExistentValue())
	{
		return PulldownBuilder::FPulldownRowColors::NonExistent;
	}

	if (IsNone())
	{
		return PulldownBuilder::FPulldownRowColors::None;
	}
		
	return DisplayTextColor;
}

#undef LOCTEXT_NAMESPACE
