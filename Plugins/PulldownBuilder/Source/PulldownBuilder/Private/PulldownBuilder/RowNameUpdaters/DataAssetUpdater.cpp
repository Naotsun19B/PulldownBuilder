// Copyright 2021-2024 Naotsun. All Rights Reserved.

#include "PulldownBuilder/RowNameUpdaters/DataAssetUpdater.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "Engine/DataAsset.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(DataAssetUpdater)
#endif

void UDataAssetUpdater::UpdateRowNamesInternal(
	UPulldownContents* PulldownContents,
	const FName& PreChangeName,
	const FName& PostChangeName
)
{
	EnumerateAssets<UDataAsset>([&](UDataAsset* DataAsset) -> bool
	{
		return UpdateMemberVariables(
			DataAsset->GetClass(),
			DataAsset,
			PulldownContents,
			PreChangeName,
			PostChangeName
		);
	});
}
