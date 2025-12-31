// Copyright 2021-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/PimplPtr.h"

class UPulldownContents;
struct FAssetData;

namespace PulldownBuilder
{
	/**
	 * A class of loader that asynchronously loads PulldownContents into editor startup to display progress notifications.
	 */
	struct PULLDOWNBUILDER_API FPulldownContentsAsyncLoader
	{
	public:
		// Registers-Unregisters a pulldown contents async loader.
		static void Register();
		static void Unregister();

		// Enumerates all PulldownContents present in the Content Browser.
		static void EnumeratePulldownContents(const TFunction<bool(UPulldownContents&)>& Callback);

		// Returns whether all PulldownContents assets has been loaded.
		static bool IsLoadedAllPulldownContents();

		// Constructor.
		FPulldownContentsAsyncLoader();

	private:
		// Called when an asset data has been added to the asset registry.
		void HandleOnAssetAdded(const FAssetData& AssetData);

		// Called when the asset registry is done loading files.
		void HandleOnFilesLoaded();

		// Start asynchronous loading of PulldownContents asset.
		void StartAsyncLoading(const FAssetData& AssetData);

		// Called when the PulldownContents asset asynchronous loading is complete.
		void EndAsyncLoading(const FAssetData AssetData);

		// Updates the display status of the progress notification.
		void UpdateProgressNotification();

	private:
		// The instance of this loader class.
		static TUniquePtr<FPulldownContentsAsyncLoader> Instance;

		// Whether all PulldownContents assets has been loaded.
		bool bIsLoadedAllPulldownContents;

		// The instance of an internal class that holds the properties used in progress notification processing.
		// In UE4.27 and earlier, there is no progress notification, so it is isolated on the source code side.
		TPimplPtr<class FProgressNotification> ProgressNotification;
	};
}
