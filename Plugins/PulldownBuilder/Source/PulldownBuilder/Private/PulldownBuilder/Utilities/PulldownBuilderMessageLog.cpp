﻿// Copyright 2022 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/PulldownBuilderMessageLog.h"
#include "PulldownBuilder/Utilities/PulldownBuilderAppearanceSettings.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "MessageLogModule.h"
#include "Logging/MessageLog.h"

namespace PulldownBuilder
{
	void FPulldownBuilderMessageLog::Register()
	{
		FMessageLogInitializationOptions Options;
		Options.bShowFilters = true;
		auto& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>(TEXT("MessageLog"));
		MessageLogModule.RegisterLogListing(
			PluginName,
			FText::FromString(FName::NameToDisplayString(PluginName.ToString(), false)),
			Options
		);
	}

	void FPulldownBuilderMessageLog::Unregister()
	{
		auto& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>(TEXT("MessageLog"));
		MessageLogModule.UnregisterLogListing(PluginName);
	}
	
	TSharedRef<FTokenizedMessage> FPulldownBuilderMessageLog::Info(const FText& InMessage)
	{
		return Message(EMessageSeverity::Info, InMessage);
	}

	TSharedRef<FTokenizedMessage> FPulldownBuilderMessageLog::Warning(const FText& InMessage)
	{
		return Message(EMessageSeverity::Warning, InMessage);
	}

	TSharedRef<FTokenizedMessage> FPulldownBuilderMessageLog::Error(const FText& InMessage)
	{
		return Message(EMessageSeverity::Error, InMessage);
	}

	TSharedRef<FTokenizedMessage> FPulldownBuilderMessageLog::Message(EMessageSeverity::Type InSeverity, const FText& InMessage)
	{
		FMessageLog MessageLog(PluginName);
		TSharedRef<FTokenizedMessage> TokenizedMessage = MessageLog.Message(InSeverity, InMessage);
		
		TOptional<EMessageSeverity::Type> MessageSeverity;
		switch (UPulldownBuilderAppearanceSettings::Get().NotificationSeverity)
		{
		case EPulldownBuilderNotificationSeverity::Info:
			MessageSeverity = EMessageSeverity::Info;
			break;
			
		case EPulldownBuilderNotificationSeverity::Warning:
			MessageSeverity = EMessageSeverity::Warning;
			break;
		
		case EPulldownBuilderNotificationSeverity::Error:
			MessageSeverity = EMessageSeverity::Error;
			break;
		
		default:
			break;
		}

		if (MessageSeverity.IsSet())
		{
			if (InSeverity <= MessageSeverity.GetValue())
			{
				MessageLog.Open();
			}
		}

		return TokenizedMessage;
	}
}
