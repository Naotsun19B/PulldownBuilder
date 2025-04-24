// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"

namespace PulldownBuilder
{
	/**
	 * A factory class that registers a custom graph pin that displays a pull-down menu for structures that inherit FPulldownStructBase.
	 */
	class PULLDOWNBUILDER_API FPulldownStructGraphPinFactory : public FGraphPanelPinFactory
	{
	public:
		// Registers-Unregisters FPulldownStructGraphPinFactory in the list of graph pin factory.
		static void Register();
		static void Unregister();

		// FGraphPanelPinFactory interface.
		virtual TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* InPin) const override;
		// End of FGraphPanelPinFactory interface.

	private:
		// The instance of this factory class.
		static TSharedPtr<FGraphPanelPinFactory> Instance;
	};
}
