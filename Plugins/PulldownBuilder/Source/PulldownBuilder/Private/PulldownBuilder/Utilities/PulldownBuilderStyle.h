// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

namespace PulldownBuilder
{
	/**
	 * A class that manages the slate icon used by this plugin.
	 */
	class PULLDOWNBUILDER_API FPulldownBuilderStyle : public FSlateStyleSet
	{	
	public:
		// Constructor.
		FPulldownBuilderStyle();
		
		// Register-Unregister and instance getter this class.
		static void Register();
		static void Unregister();
		static const ISlateStyle& Get();

	private:
		// An instance of this style class.
		static TSharedPtr<FPulldownBuilderStyle> Instance;
	};
}
