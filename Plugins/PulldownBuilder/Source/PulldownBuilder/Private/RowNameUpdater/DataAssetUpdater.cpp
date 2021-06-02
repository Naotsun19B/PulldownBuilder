// Copyright 2021 Naotsun. All Rights Reserved.

#include "RowNameUpdater/DataAssetUpdater.h"
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
