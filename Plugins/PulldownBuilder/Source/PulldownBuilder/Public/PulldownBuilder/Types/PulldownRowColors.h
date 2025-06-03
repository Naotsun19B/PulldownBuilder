// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FPulldownRow;

namespace PulldownBuilder
{
	/**
	 * A struct that defines the colors that is applied to the pull-down row.
	 */
	struct PULLDOWNBUILDER_API FPulldownRowColors
	{
	public:
		// The default color applied to general pull-down rows that have no special meaning.
		static const FSlateColor Normal;

		// The color applied to the None pull-down row that is placed at the beginning of the list in the pull-down menu.
		static const FSlateColor None;

		// The color applied to pull-down row with value that is not present in the values displayed in the list in the pull-down menu.
		static const FSlateColor NonExistent;

	public:
		// Returns the slate color registered in the style set.
		static FSlateColor FindStyleSetSlateColor(const FName& PropertyName);

		// Returns the color of the text to be displayed based on the specified pull-down row.
		static FSlateColor GetPulldownRowDisplayTextColor(const TSharedPtr<FPulldownRow>& PulldownRow);
	};
}
