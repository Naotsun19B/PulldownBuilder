// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Launch/Resources/Version.h"

/**
 * Macro to support each engine version.
 */
#if ENGINE_MINOR_VERSION == 26
#define DISABLE_SEARCH_FIELD 1
#else
#define DISABLE_SEARCH_FIELD 0
#endif
 
#if ENGINE_MINOR_VERSION <= 24
#define BEFORE_UE_4_24 1
#else
#define BEFORE_UE_4_24 0
#endif

#if ENGINE_MINOR_VERSION <= 23
#define BEFORE_UE_4_23 1
#else
#define BEFORE_UE_4_23 0
#endif

/**
 * Categories used for log output with this module.
 */
PULLDOWNBUILDER_API DECLARE_LOG_CATEGORY_EXTERN(LogPulldownBuilder, Log, All);
