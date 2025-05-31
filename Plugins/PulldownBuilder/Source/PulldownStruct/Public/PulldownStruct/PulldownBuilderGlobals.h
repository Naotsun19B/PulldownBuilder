// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EngineVersionComparison.h"

namespace PulldownBuilder
{
	namespace Global
	{
		// The name of this plugin.
		static const FName PluginName = TEXT("PulldownBuilder");

		// The delimiter between the asset name of the world to which the actor belongs and the actor's identifier,
		// used in SelectedValue in the pull-down struct constructed from the UActorNamePulldownListGenerator.
		static const FString WorldAndActorDelimiter = TEXT("::");
	}
}

/**
 * Macros to support each engine version.
 */
#ifndef UE_5_04_OR_LATER
#if !UE_VERSION_OLDER_THAN(5, 4, 0)
#define UE_5_04_OR_LATER 1
#else
#define UE_5_04_OR_LATER 0
#endif
#endif

#ifndef UE_5_03_OR_LATER
#if !UE_VERSION_OLDER_THAN(5, 3, 0)
#define UE_5_03_OR_LATER 1
#else
#define UE_5_03_OR_LATER 0
#endif
#endif

#ifndef UE_5_02_OR_LATER
#if !UE_VERSION_OLDER_THAN(5, 2, 0)
#define UE_5_02_OR_LATER 1
#else
#define UE_5_02_OR_LATER 0
#endif
#endif

#ifndef UE_5_01_OR_LATER
#if !UE_VERSION_OLDER_THAN(5, 1, 0)
#define UE_5_01_OR_LATER 1
#else
#define UE_5_01_OR_LATER 0
#endif
#endif

#ifndef UE_5_00_OR_LATER
#if !UE_VERSION_OLDER_THAN(5, 0, 0)
#define UE_5_00_OR_LATER 1
#else
#define UE_5_00_OR_LATER 0
#endif
#endif

#ifndef UE_4_26_OR_LATER
#if !UE_VERSION_OLDER_THAN(4, 26, 0)
#define UE_4_26_OR_LATER 1
#else
#define UE_4_26_OR_LATER 0
#endif
#endif

#ifndef UE_4_25_OR_LATER
#if !UE_VERSION_OLDER_THAN(4, 25, 0)
#define UE_4_25_OR_LATER 1
#else
#define UE_4_25_OR_LATER 0
#endif
#endif

/**
 * Macros that determines whether the class that defines operations on assets is UAssetDefinition or IAssetTypeActions.
 */
#ifndef ENABLE_ASSET_DEFINITION
#ifndef ENABLE_ASSET_TYPE_ACTIONS
#if (UE_5_02_OR_LATER && !PLATFORM_MAC)
#define ENABLE_ASSET_DEFINITION 1
#define ENABLE_ASSET_TYPE_ACTIONS 0
#else
#define ENABLE_ASSET_DEFINITION 0
#define ENABLE_ASSET_TYPE_ACTIONS 1
#endif
#endif
#endif

/**
 * Categories used for log output with this module.
 */
PULLDOWNSTRUCT_API DECLARE_LOG_CATEGORY_EXTERN(LogPulldownBuilder, Log, All);
