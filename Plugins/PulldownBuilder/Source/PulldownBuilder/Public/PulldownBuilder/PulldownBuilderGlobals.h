// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Version.h"

/**
 * Macro to support each engine version.
 */
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
#define BEFORE_UE_4_24 1
#else
#define BEFORE_UE_4_24 0
#endif

/**
 * Categories used for log output with this module.
 */
PULLDOWNBUILDER_API DECLARE_LOG_CATEGORY_EXTERN(LogPulldownBuilder, Log, All);
