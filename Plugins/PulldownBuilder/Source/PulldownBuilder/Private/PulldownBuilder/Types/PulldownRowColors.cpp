// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Types/PulldownRowColors.h"
#include "PulldownBuilder/Types/PulldownRow.h"
#include "PulldownBuilder/Utilities/PulldownBuilderAppearanceSettings.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#if UE_5_00_OR_LATER
#include "Styling/AppStyle.h"
#else
#include "EditorStyleSet.h"
#endif

namespace PulldownBuilder
{
	const FSlateColor FPulldownRowColors::Normal		= FindStyleSetSlateColor(TEXT("Colors.Foreground"));
	const FSlateColor FPulldownRowColors::None			= FindStyleSetSlateColor(TEXT("Colors.Warning"));
	const FSlateColor FPulldownRowColors::NonExistent	= FindStyleSetSlateColor(TEXT("Colors.Error"));

	FSlateColor FPulldownRowColors::FindStyleSetSlateColor(const FName& PropertyName)
	{
#if UE_5_01_OR_LATER
		const ISlateStyle& StyleSet = FAppStyle::Get();
#else
		const ISlateStyle& StyleSet = FEditorStyle::Get();
#endif
		return StyleSet.GetSlateColor(PropertyName);
	}

	FSlateColor FPulldownRowColors::GetPulldownRowDisplayTextColor(const TSharedPtr<FPulldownRow>& PulldownRow)
	{
		const auto& Settings = GetSettings<UPulldownBuilderAppearanceSettings>();
		if (Settings.bIsTextColoringDisabled)
		{
			return Normal;
		}
		
		if (!PulldownRow.IsValid() || PulldownRow->IsNonExistentValue())
		{
			return NonExistent;
		}

		if (PulldownRow->IsNone())
		{
			return None;
		}
		
		return PulldownRow->DisplayTextColor;
	}
}
