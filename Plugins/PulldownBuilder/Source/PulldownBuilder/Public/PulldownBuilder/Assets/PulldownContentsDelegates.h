// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UPulldownContents;

namespace PulldownBuilder
{
	/**
	 * A class that defines delegates for PulldownContents, such as registering pull-down structs and changing selected name.
	 */
	class PULLDOWNBUILDER_API FPulldownContentsDelegates
	{
	public:
		// Called when PulldownContents has been loaded.
		DECLARE_MULTICAST_DELEGATE_OneParam(FOnPulldownContentsLoaded, const UPulldownContents* /* LoadedPulldownContents */);
		static FOnPulldownContentsLoaded OnPulldownContentsLoaded;

		// Called when registers the struct set for PulldownContents in detail customization.
		DECLARE_MULTICAST_DELEGATE_OneParam(FOnDetailCustomizationRegistered, const UPulldownContents* /* RegisteredPulldownContents */);
		static FOnDetailCustomizationRegistered OnDetailCustomizationRegistered;
	
		// Called when unregisters the struct set for PulldownContents from detail customization.
		DECLARE_MULTICAST_DELEGATE_OneParam(FOnDetailCustomizationUnregistered, const UPulldownContents* /* UnregisteredPulldownContents */);
		static FOnDetailCustomizationRegistered OnDetailCustomizationUnregistered;
		
		// Called when a value added to the pull-down menu.
		DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPulldownRowAdded, UPulldownContents* /* ModifiedPulldownContents */, const FName& /* AddedSelectedValue */);
		static FOnPulldownRowAdded OnPulldownRowAdded;

		// Called when a value removed from the pull-down menu.
		DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPulldownRowRemoved, UPulldownContents* /* ModifiedPulldownContents */, const FName& /* RemovedSelectedValue */);
		static FOnPulldownRowRemoved OnPulldownRowRemoved;
		
		// Called when a value contained in the pull-down menu has been renamed.
		DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnPulldownRowRenamed, UPulldownContents* /* ModifiedPulldownContents */, const FName& /* PreChangeSelectedValue */, const FName& /* PostChangeSelectedValue */);
		static FOnPulldownRowRenamed OnPulldownRowRenamed;

		// Called when the underlying data for the pull-down menu has changed.
		DECLARE_MULTICAST_DELEGATE_OneParam(FOnPulldownContentsSourceChanged, UPulldownContents* /* ModifiedPulldownContents */);
		static FOnPulldownContentsSourceChanged OnPulldownContentsSourceChanged;
	};
};
