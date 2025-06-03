// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/CommandActions/PulldownBuilderCommands.h"
#include "PulldownBuilder/Utilities/PulldownBuilderStyle.h"

#define LOCTEXT_NAMESPACE "PulldownBuilderCommands"

namespace PulldownBuilder
{
	FPulldownBuilderCommands::FPulldownBuilderCommands()
		: TCommands<FPulldownBuilderCommands>
		(
			TEXT("PulldownBuilder"),
			LOCTEXT("ContextDesc", "Pulldown Builder"),
			NAME_None,
			FPulldownBuilderStyle::Get().GetStyleSetName()
		)
	{
	}

	void FPulldownBuilderCommands::RegisterCommands()
	{
		UI_COMMAND(DisableDisplayText, "Disable Display Text", "Displays the selected value even if it is displayed in the item in the pull-down menu.", EUserInterfaceActionType::Check, FInputChord());
		UI_COMMAND(DisableTextColoring, "Disable Text Coloring", "Disables text coloring in the pull-down menu and display text for all items in the default color.", EUserInterfaceActionType::Check, FInputChord());
	}
}

#undef LOCTEXT_NAMESPACE
