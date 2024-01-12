// Copyright 2021-2024 Naotsun. All Rights Reserved.

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
		
	private:
		// The actual registration process for this class.
		void RegisterInternal();

	public:
		// Registers-Unregisters and instance getter this class.
		static void Register();
		static void Unregister();
		static const ISlateStyle& Get();

	private:
		// The instance of this style class.
		static TUniquePtr<FPulldownBuilderStyle> Instance;
	};
}
