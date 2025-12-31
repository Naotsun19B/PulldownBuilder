// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Types/PulldownRowColors.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#if UE_5_01_OR_LATER
#include "Styling/AppStyle.h"
#else
#include "EditorStyleSet.h"
#endif

namespace PulldownBuilder
{
#if UE_5_01_OR_LATER
#define GET_SLATE_COLOR(PropertyName) FAppStyle::Get().GetSlateColor(TEXT(PropertyName))
#else
#define GET_SLATE_COLOR(PropertyName) FEditorStyle::Get().GetSlateColor(TEXT(PropertyName))
#endif
	
	const FSlateColor FPulldownRowColors::Normal		= GET_SLATE_COLOR("Colors.Foreground");
	const FSlateColor FPulldownRowColors::None			= GET_SLATE_COLOR("Colors.Warning");
	const FSlateColor FPulldownRowColors::NonExistent	= GET_SLATE_COLOR("Colors.Error");

#undef GET_SLATE_COLOR
}
