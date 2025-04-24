// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBUilder/Assets/PulldownContents.h"

class UPulldownContents;
struct FAssetData;

namespace PulldownBuilder
{
	/**
	 * A helper class that loads the PulldownContents on editor startup to register the details panel customizations for the pull-down struct.
	 */
	class PULLDOWNBUILDER_API FPulldownContentsLoader
	{
	public:
		// Called when PulldownContents has been loaded.
		DECLARE_MULTICAST_DELEGATE_OneParam(FOnPulldownContentsLoaded, const UPulldownContents* /* LoadedPulldownContents */);
		static FOnPulldownContentsLoaded OnPulldownContentsLoaded;
		
		// Called when a value added to the pull-down menu.
		DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPulldownRowAdded, UPulldownContents* /* ModifiedPulldownContents */, const FName& /* AddedRowName */);
		static FOnPulldownRowAdded OnPulldownRowAdded;

		// Called when a value removed from the pull-down menu.
		DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPulldownRowRemoved, UPulldownContents* /* ModifiedPulldownContents */, const FName& /* RemovedRowName */);
		static FOnPulldownRowRemoved OnPulldownRowRemoved;
		
		// Called when a value contained in the pull-down menu has been renamed.
		DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnPulldownRowRenamed, UPulldownContents* /* ModifiedPulldownContents */, const FName& /* PreChangeName */, const FName& /* PostChangeName */);
		static FOnPulldownRowRenamed OnPulldownRowRenamed;

		// Called when the underlying data for the pull-down menu has changed.
		DECLARE_MULTICAST_DELEGATE_OneParam(FOnPulldownContentsSourceChanged, UPulldownContents* /* ModifiedPulldownContents */);
		static FOnPulldownContentsSourceChanged OnPulldownContentsSourceChanged;
		
	public:
		// Registers-Unregisters a pulldown contents loader.
		static void Register();
		static void Unregister();
		
	private:
		// Called when an asset data has been added to the asset registry.
		static void HandleOnAssetAdded(const FAssetData& AssetData);

		// Called when PulldownContents has been loaded.
		static void HandleOnPulldownContentsLoaded(const UPulldownContents* LoadedPulldownContents);

		// Called when a value added to the pull-down menu.
		static void HandleOnPulldownRowAdded(UPulldownContents* ModifiedPulldownContents, const FName& AddedRowName);

		// Called when a value removed from the pull-down menu.
		static void HandleOnPulldownRowRemoved(UPulldownContents* ModifiedPulldownContents, const FName& RemovedRowName);
		
		// Called when a value contained in the pull-down menu has been renamed.
		static void HandleOnPulldownRowRenamed(
			UPulldownContents* ModifiedPulldownContents,
			const FName& PreChangeName,
			const FName& PostChangeName
		);

		// Called when the underlying data for the pull-down menu has changed.
		static void HandleOnPulldownContentsSourceChanged(UPulldownContents* ModifiedPulldownContents);

	private:
		// Handles for each event.
		static FDelegateHandle OnAssetAddedHandle;
		static FDelegateHandle OnPulldownContentsLoadedHandle;
		static FDelegateHandle OnPulldownRowAddedHandle;
		static FDelegateHandle OnPulldownRowRemovedHandle;
		static FDelegateHandle OnPulldownRowRenamedHandle;
		static FDelegateHandle OnPulldownContentsSourceChangedHandle;
	};
};
