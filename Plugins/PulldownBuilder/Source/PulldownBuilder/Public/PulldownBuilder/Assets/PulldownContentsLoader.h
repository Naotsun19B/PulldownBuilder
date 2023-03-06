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

		// Called when a value added to or removed from the pulldown menu.
		DECLARE_MULTICAST_DELEGATE_OneParam(FOnPulldownRowAddedOrRemoved, UPulldownContents* /* ModifiedPulldownContents */);
		static FOnPulldownRowAddedOrRemoved OnPulldownRowAddedOrRemoved;
		
		// Called when a value contained in the pulldown menu has been renamed.
		DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnPulldownRowChanged, UPulldownContents* /* ModifiedPulldownContents */, const FName& /* PreChangeName */, const FName& /* PostChangeName */);
		static FOnPulldownRowChanged OnPulldownRowChanged;
		
	public:
		// Register a pulldown contents loader.
		static void Register();
		
	private:
		// Called when an asset data has been added to the asset registry.
		static void HandleOnAssetAdded(const FAssetData& AssetData);
	};
};
