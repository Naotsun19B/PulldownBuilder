// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Assets/PulldownContentsAsyncLoader.h"
#include "PulldownBuilder/Assets/PulldownContentsDelegates.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#if UE_4_26_OR_LATER
#include "AssetRegistry/IAssetRegistry.h"
#else
#include "IAssetRegistry.h"
#endif
#include "AssetRegistry/AssetData.h"
#include "Engine/AssetManager.h"
#if UE_5_00_OR_LATER
#include "Framework/Notifications/NotificationManager.h"
#endif

#define LOCTEXT_NAMESPACE "PulldownContentsAsyncLoader"

namespace PulldownBuilder
{
	namespace AssetRegistry
	{
		IAssetRegistry* Get()
		{
#if UE_4_26_OR_LATER
			return IAssetRegistry::Get();
#else
			if (const auto* AssetRegistryModule = FModuleManager::LoadModulePtr<FAssetRegistryModule>(TEXT("AssetRegistry")))
			{
				return &AssetRegistryModule->Get();
			}

			return nullptr;
#endif
		}
	}
	
#if UE_5_00_OR_LATER
	class FProgressNotification
	{
	public:
		// The handle for the progress notification for loading PulldownContents assets.
		FProgressNotificationHandle NotificationHandle;

		// The list of loaded PulldownContents assets.
		TArray<FAssetData> PulldownContentsToLoad;
	};
#endif
	
	void FPulldownContentsAsyncLoader::Register()
	{
		Instance = MakeUnique<FPulldownContentsAsyncLoader>();
		check(Instance.IsValid());

#if UE_5_00_OR_LATER
		Instance->ProgressNotification = MakePimpl<FProgressNotification>();
#endif
		
		if (IAssetRegistry* AssetRegistry = AssetRegistry::Get())
		{
			AssetRegistry->OnAssetAdded().AddRaw(Instance.Get(), &FPulldownContentsAsyncLoader::HandleOnAssetAdded);
			AssetRegistry->OnFilesLoaded().AddRaw(Instance.Get(), &FPulldownContentsAsyncLoader::HandleOnFilesLoaded);
		}
	}

	void FPulldownContentsAsyncLoader::Unregister()
	{
		if (IAssetRegistry* AssetRegistry = AssetRegistry::Get())
		{
			AssetRegistry->OnFilesLoaded().RemoveAll(Instance.Get());
			AssetRegistry->OnAssetAdded().RemoveAll(Instance.Get());
		}

		Instance.Reset();
	}

	void FPulldownContentsAsyncLoader::EnumeratePulldownContents(const TFunction<bool(UPulldownContents&)>& Callback)
	{
		const UClass* PulldownContentsClass = UPulldownContents::StaticClass();
		check(IsValid(PulldownContentsClass));
		
		IAssetRegistry* AssetRegistry = AssetRegistry::Get();
		if (AssetRegistry == nullptr)
		{
			return;
		}

		TArray<FAssetData> AssetDataList;
#if UE_5_01_OR_LATER
		const FTopLevelAssetPath& ClassPathName = PulldownContentsClass->GetClassPathName();
		if (!AssetRegistry->GetAssetsByClass(ClassPathName, AssetDataList))
#else
		if (!AssetRegistry->GetAssetsByClass(PulldownContentsClass->GetFName(), AssetDataList))
#endif
		{
			return;
		}

		for (const auto& AssetData : AssetDataList)
		{
			if (auto* PulldownContents = Cast<UPulldownContents>(AssetData.GetAsset()))
			{
				Callback(*PulldownContents);
			}
		}
	}

	bool FPulldownContentsAsyncLoader::IsLoadedAllPulldownContents()
	{
		if (!Instance.IsValid())
		{
			return false;
		}
		
		return Instance->bIsLoadedAllPulldownContents;
	}

	FPulldownContentsAsyncLoader::FPulldownContentsAsyncLoader()
		: bIsLoadedAllPulldownContents(false)
	{
	}

	void FPulldownContentsAsyncLoader::HandleOnAssetAdded(const FAssetData& AssetData)
	{
		const UClass* PulldownContentsClass = UPulldownContents::StaticClass();
		check(IsValid(PulldownContentsClass));
#if UE_5_01_OR_LATER
		if (AssetData.AssetClassPath != PulldownContentsClass->GetClassPathName())
#else
		if (AssetData.AssetClass == PulldownContentsClass->GetFName())
#endif
		{
			return;
		}
		
		StartAsyncLoading(AssetData);
	}

	void FPulldownContentsAsyncLoader::HandleOnFilesLoaded()
	{
		bIsLoadedAllPulldownContents = true;
		FPulldownContentsDelegates::OnAllPulldownContentsLoaded.Broadcast();
	}

	void FPulldownContentsAsyncLoader::StartAsyncLoading(const FAssetData& AssetData)
	{
#if UE_5_00_OR_LATER
		check(ProgressNotification.IsValid());
		ProgressNotification->PulldownContentsToLoad.Add(AssetData);
		UpdateProgressNotification();
#endif
				
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			AssetData.ToSoftObjectPath(),
			FStreamableDelegate::CreateRaw(this, &FPulldownContentsAsyncLoader::EndAsyncLoading, AssetData),
			FStreamableManager::AsyncLoadHighPriority
		);
	}

	void FPulldownContentsAsyncLoader::EndAsyncLoading(const FAssetData AssetData)
	{
#if UE_5_00_OR_LATER
		check(ProgressNotification.IsValid());
		ProgressNotification->PulldownContentsToLoad.Remove(AssetData);
		UpdateProgressNotification();
#endif

		if (const auto* LoadedPulldownContents = Cast<UPulldownContents>(AssetData.GetAsset()))
		{
			FPulldownContentsDelegates::OnPulldownContentsLoaded.Broadcast(LoadedPulldownContents);
			
			UE_LOG(LogPulldownBuilder, Log, TEXT("Loaded %s"), *LoadedPulldownContents->GetName());
		}
	}

	void FPulldownContentsAsyncLoader::UpdateProgressNotification()
	{
#if UE_5_00_OR_LATER
		check(ProgressNotification.IsValid());
		
		const int32 TotalWorkToDo = ProgressNotification->PulldownContentsToLoad.Num();
		const FText DisplayText = FText::Format(
			LOCTEXT("ProgressNotificationLabel", "Loading Pulldown Contents ({0})"),
			TotalWorkToDo
		);
		auto& SlateNotificationManager = FSlateNotificationManager::Get();
		if (!ProgressNotification->NotificationHandle.IsValid())
		{
			ProgressNotification->NotificationHandle = SlateNotificationManager.StartProgressNotification(
				DisplayText,
				TotalWorkToDo
			);
		}
		else
		{
			SlateNotificationManager.UpdateProgressNotification(
				ProgressNotification->NotificationHandle,
				1,
				TotalWorkToDo,
				DisplayText
			);
		}
#endif
	}

	TUniquePtr<FPulldownContentsAsyncLoader> FPulldownContentsAsyncLoader::Instance;
}

#undef LOCTEXT_NAMESPACE
