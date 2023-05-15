// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Assets/PulldownContentsLoader.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownBuilder/RowNameUpdaters/RowNameUpdaterBase.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#if UE_4_26_OR_LATER
#include "AssetRegistry/IAssetRegistry.h"
#else
#include "IAssetRegistry.h"
#endif

namespace PulldownBuilder
{
#if UE_4_26_OR_LATER
	namespace PulldownContentsLoaderUtils
	{
		
	}
#endif
	
	FPulldownContentsLoader::FOnPulldownContentsLoaded FPulldownContentsLoader::OnPulldownContentsLoaded;
	FPulldownContentsLoader::FOnPulldownRowAdded FPulldownContentsLoader::OnPulldownRowAdded;
	FPulldownContentsLoader::FOnPulldownRowRemoved FPulldownContentsLoader::OnPulldownRowRemoved;
	FPulldownContentsLoader::FOnPulldownRowRenamed FPulldownContentsLoader::OnPulldownRowRenamed;
	FPulldownContentsLoader::FOnPulldownContentsSourceChanged FPulldownContentsLoader::OnPulldownContentsSourceChanged;
	
	void FPulldownContentsLoader::Register()
	{
		IAssetRegistry* AssetRegistry = FPulldownBuilderUtils::GetAssetRegistry();
		if (AssetRegistry == nullptr)
		{
			return;
		}
		
		OnAssetAddedHandle = AssetRegistry->OnAssetAdded().AddStatic(&FPulldownContentsLoader::HandleOnAssetAdded);

		OnPulldownContentsLoadedHandle = OnPulldownContentsLoaded.AddStatic(&FPulldownContentsLoader::HandleOnPulldownContentsLoaded);
		OnPulldownRowAddedHandle = OnPulldownRowAdded.AddStatic(&FPulldownContentsLoader::HandleOnPulldownRowAdded);
		OnPulldownRowRemovedHandle = OnPulldownRowRemoved.AddStatic(&FPulldownContentsLoader::HandleOnPulldownRowRemoved);
		OnPulldownRowRenamedHandle = OnPulldownRowRenamed.AddStatic(&FPulldownContentsLoader::HandleOnPulldownRowRenamed);
		OnPulldownContentsSourceChangedHandle = OnPulldownContentsSourceChanged.AddStatic(&FPulldownContentsLoader::HandleOnPulldownContentsSourceChanged);
	}

	void FPulldownContentsLoader::Unregister()
	{
		OnPulldownContentsSourceChanged.Remove(OnPulldownContentsSourceChangedHandle);
		OnPulldownRowRenamed.Remove(OnPulldownRowRenamedHandle);
		OnPulldownRowRemoved.Remove(OnPulldownRowRemovedHandle);
		OnPulldownRowAdded.Remove(OnPulldownRowAddedHandle);
		OnPulldownContentsLoaded.Remove(OnPulldownContentsLoadedHandle);

		if (auto* AssetRegistry = FPulldownBuilderUtils::GetAssetRegistry())
		{
			AssetRegistry->OnAssetAdded().Remove(OnAssetAddedHandle);
		}
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
		if (IsValid(ModifiedPulldownContents))
		{
			UE_LOG(LogPulldownBuilder, Log, TEXT("Detected renaming of row name that is the source of %s. (%s -> %s)"), *ModifiedPulldownContents->GetName(), *PreChangeName.ToString(), *PostChangeName.ToString());
		}
		
		URowNameUpdaterBase::UpdateRowNames(ModifiedPulldownContents, PreChangeName, PostChangeName);
	}

	void FPulldownContentsLoader::HandleOnPulldownContentsSourceChanged(UPulldownContents* ModifiedPulldownContents)
	{
		if (!IsValid(ModifiedPulldownContents))
		{
			return;
		}
		
		UE_LOG(LogPulldownBuilder, Log, TEXT("Detected modification of underlying data of %s."), *ModifiedPulldownContents->GetName());
	}

	FDelegateHandle FPulldownContentsLoader::OnAssetAddedHandle;
	FDelegateHandle FPulldownContentsLoader::OnPulldownContentsLoadedHandle;
	FDelegateHandle FPulldownContentsLoader::OnPulldownRowAddedHandle;
	FDelegateHandle FPulldownContentsLoader::OnPulldownRowRemovedHandle;
	FDelegateHandle FPulldownContentsLoader::OnPulldownRowRenamedHandle;
	FDelegateHandle FPulldownContentsLoader::OnPulldownContentsSourceChangedHandle;
}
