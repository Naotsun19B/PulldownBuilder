// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBUilder/Assets/PulldownContents.h"

class UPulldownContents;
struct FAssetData;

namespace PulldownBuilder
{
	/**
	 * Helper class that loads the PulldownContents on editor startup to
	 * register the details panel customizations for the pulldown struct.
	 */
	class PULLDOWNBUILDER_API FPulldownContentsLoader
	{
	public:
		// Called when PulldownContents has been loaded.
		DECLARE_MULTICAST_DELEGATE_OneParam(FOnPulldownContentsLoaded, const UPulldownContents* /* LoadedPulldownContents */);
		static FOnPulldownContentsLoaded OnPulldownContentsLoaded;
		
		// Called when a value added to the pulldown menu.
		DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPulldownRowAdded, UPulldownContents* /* ModifiedPulldownContents */, const FName& /* AddedRowName */);
		static FOnPulldownRowAdded OnPulldownRowAdded;

		// Called when a value removed from the pulldown menu.
		DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPulldownRowRemoved, UPulldownContents* /* ModifiedPulldownContents */, const FName& /* RemovedRowName */);
		static FOnPulldownRowRemoved OnPulldownRowRemoved;
		
		// Called when a value contained in the pulldown menu has been renamed.
		DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnPulldownRowRenamed, UPulldownContents* /* ModifiedPulldownContents */, const FName& /* PreChangeName */, const FName& /* PostChangeName */);
		static FOnPulldownRowRenamed OnPulldownRowRenamed;
		
	public:
		// Register a pulldown contents loader.
		static void Register();
		
	private:
		// Called when an asset data has been added to the asset registry.
		static void HandleOnAssetAdded(const FAssetData& AssetData);

		// Called when PulldownContents has been loaded.
		static void HandleOnPulldownContentsLoaded(const UPulldownContents* LoadedPulldownContents);

		// Called when a value added to the pulldown menu.
		static void HandleOnPulldownRowAdded(UPulldownContents* ModifiedPulldownContents, const FName& AddedRowName);

		// Called when a value removed from the pulldown menu.
		static void HandleOnPulldownRowRemoved(UPulldownContents* ModifiedPulldownContents, const FName& RemovedRowName);
		
		// Called when a value contained in the pulldown menu has been renamed.
		static void HandleOnPulldownRowRenamed(
			UPulldownContents* ModifiedPulldownContents,
			const FName& PreChangeName,
			const FName& PostChangeName
		);
	};
};
