// Copyright 2021-2025 Naotsun. All Rights Reserved.

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
#include "Engine/AssetManager.h"
#include "Framework/Notifications/NotificationManager.h"

#define LOCTEXT_NAMESPACE "PulldownContentsLoader"

namespace PulldownBuilder
{
	namespace PulldownContentsLoader
	{
		struct FAsyncLoadProgressNotificationHelper
		{
		public:
			void StartAsyncLoading(const FAssetData& AssetData)
			{
#if UE_5_00_OR_LATER
				PulldownContentsToLoad.Add(AssetData);
				UpdateProgressNotification();
#endif
				
				UAssetManager::GetStreamableManager().RequestAsyncLoad(
					AssetData.ToSoftObjectPath(),
					FStreamableDelegate::CreateRaw(this, &FAsyncLoadProgressNotificationHelper::EndAsyncLoading, AssetData),
					FStreamableManager::AsyncLoadHighPriority
				);
			}

		private:
			void EndAsyncLoading(const FAssetData AssetData)
			{
#if UE_5_00_OR_LATER
				PulldownContentsToLoad.Remove(AssetData);
				UpdateProgressNotification();
#endif

				if (const auto* PulldownContents = Cast<UPulldownContents>(AssetData.GetAsset()))
				{
					FPulldownContentsLoader::OnPulldownContentsLoaded.Broadcast(PulldownContents);	
				}
			}

#if UE_5_00_OR_LATER
			void UpdateProgressNotification()
			{
				const int32 TotalWorkToDo = PulldownContentsToLoad.Num();
				const FText DisplayText = FText::Format(
					LOCTEXT("ProgressNotificationLabel", "Loading Pulldown Contents ({0})"),
					TotalWorkToDo
				);
				auto& SlateNotificationManager = FSlateNotificationManager::Get();
				if (!NotificationHandle.IsValid())
				{
					NotificationHandle = SlateNotificationManager.StartProgressNotification(
						DisplayText,
						TotalWorkToDo
					);
				}
				else
				{
					SlateNotificationManager.UpdateProgressNotification(
						NotificationHandle,
						1,
						TotalWorkToDo,
						DisplayText
					);
				}
			};
			
		private:
			FProgressNotificationHandle NotificationHandle;
			TArray<FAssetData> PulldownContentsToLoad;
#endif
		};

		static FAsyncLoadProgressNotificationHelper AsyncLoadProgressNotificationHelper;
	}
	
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
		
		PulldownContentsLoader::AsyncLoadProgressNotificationHelper.StartAsyncLoading(AssetData);
	}

	void FPulldownContentsLoader::HandleOnPulldownContentsLoaded(const UPulldownContents* LoadedPulldownContents)
	{
		if (!IsValid(LoadedPulldownContents))
		{
			return;
		}
		
		UE_LOG(LogPulldownBuilder, Log, TEXT("Loaded %s"), *LoadedPulldownContents->GetName());
	}

	void FPulldownContentsLoader::HandleOnPulldownRowAdded(UPulldownContents* ModifiedPulldownContents, const FName& AddedSelectedValue)
	{
		if (!IsValid(ModifiedPulldownContents))
		{
			return;
		}

		UE_LOG(LogPulldownBuilder, Log, TEXT("Detected the addition of the row name that is the source of %s. (+ %s)"), *ModifiedPulldownContents->GetName(), *AddedSelectedValue.ToString());
	}

	void FPulldownContentsLoader::HandleOnPulldownRowRemoved(UPulldownContents* ModifiedPulldownContents, const FName& RemovedSelectedValue)
	{
		if (!IsValid(ModifiedPulldownContents))
		{
			return;
		}

		UE_LOG(LogPulldownBuilder, Log, TEXT("Detected removal of the row name that is the source of %s. (- %s)"), *ModifiedPulldownContents->GetName(), *RemovedSelectedValue.ToString());
	}
	
	void FPulldownContentsLoader::HandleOnPulldownRowRenamed(
		UPulldownContents* ModifiedPulldownContents,
		const FName& PreChangeSelectedValue,
		const FName& PostChangeSelectedValue
	)
	{
		if (IsValid(ModifiedPulldownContents))
		{
			UE_LOG(LogPulldownBuilder, Log, TEXT("Detected renaming of row name that is the source of %s. (%s -> %s)"), *ModifiedPulldownContents->GetName(), *PreChangeSelectedValue.ToString(), *PostChangeSelectedValue.ToString());
		}
		
		URowNameUpdaterBase::UpdateRowNames(ModifiedPulldownContents, PreChangeSelectedValue, PostChangeSelectedValue);
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

#undef LOCTEXT_NAMESPACE
