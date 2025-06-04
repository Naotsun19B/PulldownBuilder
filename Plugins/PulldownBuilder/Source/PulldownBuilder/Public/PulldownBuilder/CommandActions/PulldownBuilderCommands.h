// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Framework/Commands/UICommandInfo.h"

namespace PulldownBuilder
{
	/**
	 * A class to register the shortcut key used in this plugin.
	 */
	class PULLDOWNBUILDER_API FPulldownBuilderCommands : public TCommands<FPulldownBuilderCommands>
	{
	public:
		// Constructor.
		FPulldownBuilderCommands();

		// TCommands interface.
		virtual void RegisterCommands() override;
		// End of TCommands interface.
		
	public:
		// Instances of bound commands.
		TSharedPtr<FUICommandInfo> DisableDisplayText;
		TSharedPtr<FUICommandInfo> DisableTextColoring;
	};
}
