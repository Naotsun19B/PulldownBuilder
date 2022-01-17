// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownBuilder/RowNameUpdaters/DataAssetUpdater.h"
#include "Engine/DataAsset.h"

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
