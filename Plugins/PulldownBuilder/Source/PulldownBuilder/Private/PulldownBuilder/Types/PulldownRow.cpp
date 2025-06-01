// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Types/PulldownRow.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#if UE_5_00_OR_LATER
#include "Styling/AppStyle.h"
#else
#include "EditorStyleSet.h"
#endif

#define LOCTEXT_NAMESPACE "PulldownRow"

FPulldownRow::FPulldownRow()
	: FPulldownRow(FName(NAME_None).ToString())
{
}

FPulldownRow::FPulldownRow(const FString& InSelectedValue, const FText& InTooltipText, const FText& InDisplayText)
	: SelectedValue(InSelectedValue)
	, DisplayText(InDisplayText)
	, TooltipText(InTooltipText)
	, bIsDefaultValue(false)
	, DisplayTextColor(
#if UE_5_00_OR_LATER
		FAppStyle::Get()
#else
		FEditorStyle::Get()
#endif
		.GetSlateColor(TEXT("Colors.Foreground")).GetSpecifiedColor()
	)
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
