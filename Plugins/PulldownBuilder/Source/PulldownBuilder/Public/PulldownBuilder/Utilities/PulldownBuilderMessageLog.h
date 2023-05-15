// Copyright 2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Logging/TokenizedMessage.h"

namespace PulldownBuilder
{
	/**
	 * A custom message log class used by this plugin.
	 */
	class PULLDOWNBUILDER_API FPulldownBuilderMessageLog
	{
	public:
		// Registers-Unregisters in the custom message log class.
		static void Register();
		static void Unregister();
		
		// Outputs a message to the message log for each severity.
		static TSharedRef<FTokenizedMessage> Info(const FText& InMessage);
		static TSharedRef<FTokenizedMessage> Warning(const FText& InMessage);
		static TSharedRef<FTokenizedMessage> Error(const FText& InMessage);

	private:
		// Internal function that post message to message log and open message log, if specified severity is higher than editor preferences.
		static TSharedRef<FTokenizedMessage> Message(const EMessageSeverity::Type InSeverity, const FText& InMessage);
	};
}
