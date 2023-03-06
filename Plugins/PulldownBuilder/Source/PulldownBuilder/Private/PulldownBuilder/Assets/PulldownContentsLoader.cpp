// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Assets/PulldownContentsLoader.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/RowNameUpdaters/RowNameUpdaterBase.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#if UE_4_26_OR_LATER
#include "AssetRegistry/IAssetRegistry.h"
#else
#include "Modules/ModuleManager.h"
#include "AssetRegistryModule.h"
#include "IAssetRegistry.h"
#endif

namespace PulldownBuilder
{
	FPulldownContentsLoader::FOnPulldownContentsLoaded FPulldownContentsLoader::OnPulldownContentsLoaded;
	FPulldownContentsLoader::FOnPulldownRowAddedOrRemoved FPulldownContentsLoader::OnPulldownRowAddedOrRemoved;
	FPulldownContentsLoader::FOnPulldownRowChanged FPulldownContentsLoader::OnPulldownRowChanged;
	
	void FPulldownContentsLoader::Register()
	{
#if UE_4_26_OR_LATER
		auto& AssetRegistry = IAssetRegistry::GetChecked();
#else
		const auto& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		auto& AssetRegistry = AssetRegistryModule.Get();
#endif
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
