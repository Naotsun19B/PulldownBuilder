// Copyright 2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Logging/TokenizedMessage.h"
#include "PulldownBuilder/Types/PulldownBuilderNotificationSeverity.h"

namespace PulldownBuilder
{
	/**
	 * Custom message log class used by this plugin.
	 */
	class PULLDOWNBUILDER_API FPulldownBuilderMessageLog
	{
	public:
		// Register - unregister in the custom message log class.
		static void Register();
		static void Unregister();
		
		// Output a message to the message log for each severity.
		static TSharedRef<FTokenizedMessage> Info(const FText& InMessage);
		static TSharedRef<FTokenizedMessage> Warning(const FText& InMessage);
		static TSharedRef<FTokenizedMessage> Error(const FText& InMessage);

	private:
		// Internal function that post message to message log and open message log, if specified severity is higher than editor preferences.
		static TSharedRef<FTokenizedMessage> Message(EMessageSeverity::Type InSeverity, const FText& InMessage);
	};
}
