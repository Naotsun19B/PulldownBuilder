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

#define LOCTEXT_NAMESPACE "PulldownContentsLoader"

namespace PulldownBuilder
{
	FPulldownContentsLoader::FOnPulldownContentsLoaded FPulldownContentsLoader::OnPulldownContentsLoaded;
	FPulldownContentsLoader::FOnPulldownRowAdded FPulldownContentsLoader::OnPulldownRowAdded;
	FPulldownContentsLoader::FOnPulldownRowRemoved FPulldownContentsLoader::OnPulldownRowRemoved;
	FPulldownContentsLoader::FOnPulldownRowRenamed FPulldownContentsLoader::OnPulldownRowRenamed;
	
	void FPulldownContentsLoader::Register()
	{
#if UE_4_26_OR_LATER
		auto& AssetRegistry = IAssetRegistry::GetChecked();
#else
		const auto& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		auto& AssetRegistry = AssetRegistryModule.Get();
#endif
		AssetRegistry.OnAssetAdded().AddStatic(&FPulldownContentsLoader::HandleOnAssetAdded);

		OnPulldownContentsLoaded.AddStatic(&FPulldownContentsLoader::HandleOnPulldownContentsLoaded);
		OnPulldownRowAdded.AddStatic(&FPulldownContentsLoader::HandleOnPulldownRowAdded);
		OnPulldownRowRemoved.AddStatic(&FPulldownContentsLoader::HandleOnPulldownRowRemoved);
		OnPulldownRowRenamed.AddStatic(&FPulldownContentsLoader::HandleOnPulldownRowRenamed);
	
		OnPulldownRowRenamed.AddStatic(&URowNameUpdaterBase::UpdateRowNames);
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
			
			OnPulldownContentsLoaded.Broadcast(PulldownContents);
		}
	}

	void FPulldownContentsLoader::HandleOnPulldownContentsLoaded(const UPulldownContents* LoadedPulldownContents)
	{
		if (!IsValid(LoadedPulldownContents))
		{
			return;
		}
		
		UE_LOG(LogPulldownBuilder, Log, TEXT("Loaded %s"), *LoadedPulldownContents->GetName());
	}

	void FPulldownContentsLoader::HandleOnPulldownRowAdded(UPulldownContents* ModifiedPulldownContents, const FName& AddedRowName)
	{
		if (!IsValid(ModifiedPulldownContents))
		{
			return;
		}

		UE_LOG(LogPulldownBuilder, Log, TEXT("Detected the addition of the row name that is the source of %s. (+ %s)"), *ModifiedPulldownContents->GetName(), *AddedRowName.ToString());
	}

	void FPulldownContentsLoader::HandleOnPulldownRowRemoved(UPulldownContents* ModifiedPulldownContents, const FName& RemovedRowName)
	{
		if (!IsValid(ModifiedPulldownContents))
		{
			return;
		}

		UE_LOG(LogPulldownBuilder, Log, TEXT("Detected removal of the row name that is the source of %s. (- %s)"), *ModifiedPulldownContents->GetName(), *RemovedRowName.ToString());
	}

	void FPulldownContentsLoader::HandleOnPulldownRowRenamed(
		UPulldownContents* ModifiedPulldownContents,
		const FName& PreChangeName,
		const FName& PostChangeName
	)
	{
		if (!IsValid(ModifiedPulldownContents))
		{
			return;
		}

		UE_LOG(LogPulldownBuilder, Log, TEXT("Detected renaming of row name that is the source of %s. (%s -> %s)"), *ModifiedPulldownContents->GetName(), *PreChangeName.ToString(), *PostChangeName.ToString());
	}
}

#undef LOCTEXT_NAMESPACE
