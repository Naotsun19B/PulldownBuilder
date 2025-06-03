// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/PulldownBuilderRedirectSettings.h"
#include "PulldownBuilder/RowNameUpdaters/BlueprintUpdater.h"
#include "PulldownBuilder/RowNameUpdaters/DataAssetUpdater.h"
#include "PulldownBuilder/RowNameUpdaters/DataTableUpdater.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(PulldownBuilderRedirectSettings)
#endif

UPulldownBuilderRedirectSettings::UPulldownBuilderRedirectSettings()
	: bShouldUpdateWhenSourceRowNameChanged(false)
{
	ActiveRowNameUpdater.Add(UBlueprintUpdater::StaticClass());
	ActiveRowNameUpdater.Add(UDataTableUpdater::StaticClass());
	ActiveRowNameUpdater.Add(UDataAssetUpdater::StaticClass());
}

FString UPulldownBuilderRedirectSettings::GetSettingsName() const
{
	return TEXT("Redirect");
}

