// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Assets/PulldownContentsLoader.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/RowNameUpdaters/RowNameUpdaterBase.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "AssetRegistry/IAssetRegistry.h"

namespace PulldownBuilder
{
	FPulldownContentsLoader::FOnPulldownContentsLoaded FPulldownContentsLoader::OnPulldownContentsLoaded;
	FPulldownContentsLoader::FOnPulldownRowAddedOrRemoved FPulldownContentsLoader::OnPulldownRowAddedOrRemoved;
	FPulldownContentsLoader::FOnPulldownRowChanged FPulldownContentsLoader::OnPulldownRowChanged;
	
	void FPulldownContentsLoader::Register()
	{
		auto& AssetRegistry = IAssetRegistry::GetChecked();
		AssetRegistry.OnAssetAdded().AddStatic(&FPulldownContentsLoader::HandleOnAssetAdded);
		
		OnPulldownRowChanged.AddStatic(&URowNameUpdaterBase::UpdateRowNames);
	}

	void FPulldownContentsLoader::HandleOnAssetAdded(const FAssetData& AssetData)
	{
		const UClass* Class = AssetData.GetClass();
		if (!IsValid(Class))
		{
			return;
		}

		if (!Class->IsChildOf<UPulldownContents>())
		{
			return;
		}

		if (const auto* PulldownContents = Cast<UPulldownContents>(AssetData.GetAsset()))
		{
			UE_LOG(LogPulldownBuilder, Log, TEXT("Loaded %s"), *PulldownContents->GetName());
			OnPulldownContentsLoaded.Broadcast(PulldownContents);
		}
	}
}
