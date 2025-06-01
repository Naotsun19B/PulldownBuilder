// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Types/PulldownRow.h"

#define LOCTEXT_NAMESPACE "PulldownRow"

FPulldownRow::FPulldownRow()
	: SelectedValue(FName(NAME_None).ToString())
	, bIsDefaultValue(false)
	, bIsNonExistentValue(false)
{
}

FPulldownRow::FPulldownRow(const FString& InSelectedValue, const FText& InTooltipText, const FText& InDisplayText)
	: SelectedValue(InSelectedValue)
	, DisplayText(InDisplayText)
	, TooltipText(InTooltipText)
	, bIsDefaultValue(false)
	, bIsNonExistentValue(false)
{
}

bool FPulldownRow::IsNone() const
{
	return (SelectedValue == FName(NAME_None).ToString());
}

FText FPulldownRow::GetDisplayText() const
{
	return (DisplayText.IsEmpty() ? FText::FromString(SelectedValue) : DisplayText);
}

bool FPulldownRow::IsNonExistentValue() const
{
	return bIsNonExistentValue;
}

#undef LOCTEXT_NAMESPACE
