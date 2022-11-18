// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Version.h"

namespace PulldownBuilder
{
	// The name of this plugin.
	static const FName PluginName = TEXT("PulldownBuilder");
}

/**
 * Macro to support each engine version.
 */
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION <= 0) || ENGINE_MAJOR_VERSION < 5
#define BEFORE_UE_5_00 1
#else
#define BEFORE_UE_5_00 0
#endif

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 27
#define BEFORE_UE_4_27 1
#else
#define BEFORE_UE_4_27 0
#endif

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
#define BEFORE_UE_4_24 1
#else
#define BEFORE_UE_4_24 0
#endif

/**
 * Categories used for log output with this module.
 */
PULLDOWNSTRUCT_API DECLARE_LOG_CATEGORY_EXTERN(LogPulldownBuilder, Log, All);
