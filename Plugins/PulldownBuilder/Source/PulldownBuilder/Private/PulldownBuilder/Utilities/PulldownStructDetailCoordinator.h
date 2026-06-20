// Copyright 2021-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/WeakObjectPtr.h"
#include "UObject/PackageReload.h"
#include "PulldownBuilder/Types/PulldownStructType.h"

class UPulldownContents;

namespace PulldownBuilder
{
	/**
	 * A module-scoped coordinator that owns the lifecycle of FPulldownStructDetail registrations.
	 *
	 * Before this coordinator existed, UPulldownContents itself called FPulldownStructDetail::Register /
	 * Unregister from PostLoad, BeginDestroy, PreEditChange, PostEditChange, and HandleOnPackageReloaded.
	 * That created a hard dependency from a data asset (UPulldownContents) onto an editor-UI subsystem
	 * (PropertyEditor + FPulldownStructDetail), made the asset non-testable in isolation, and produced
	 * an asymmetric register/unregister surface that was easy to break.
	 *
	 * This class collects the same responsibility in one place:
	 *   - Listens to PulldownContents lifecycle delegates (loaded, struct-type changing/changed).
	 *   - Listens to package reloads to refresh registrations.
	 *   - Tracks which struct types are currently registered so unregister always matches register.
	 *
	 * UPulldownContents now only fires lifecycle delegates; it never touches FPulldownStructDetail directly.
	 */
	class FPulldownStructDetailCoordinator
	{
	public:
		// Module-scoped lifetime mirror of FPulldownBuilderModule::StartupModule / ShutdownModule.
		static void Register();
		static void Unregister();

		// Public destructor required so TUniquePtr's default deleter can call it from outside the class.
		// The class itself is still effectively a singleton -- construction is private.
		~FPulldownStructDetailCoordinator();

	private:
		// Constructor.
		FPulldownStructDetailCoordinator();

		// Wire / unwire the delegate listeners. Called by Register / Unregister.
		void Bind();
		void Unbind();

		// Delegate callbacks. Each translates a PulldownContents lifecycle event into a
		// FPulldownStructDetail::Register / Unregister pair, keeping the StructType -> Owner map consistent.
		void HandleOnPulldownContentsLoaded(const UPulldownContents* LoadedPulldownContents);
		void HandleOnAllPulldownContentsLoaded();
		void HandleOnPulldownStructTypeChangePending(UPulldownContents* PulldownContentsAboutToChange);
		void HandleOnPulldownStructTypeChangeCommitted(UPulldownContents* ChangedPulldownContents);
		void HandleOnPackageReloaded(EPackageReloadPhase ReloadPhase, FPackageReloadedEvent* ReloadedEvent);

		// Performs Register / Unregister and updates RegisteredStructTypes.
		// No-ops gracefully on invalid PulldownContents or empty StructType.
		void RegisterFor(const UPulldownContents* PulldownContents);
		void UnregisterFor(const UPulldownContents* PulldownContents);
		void UnregisterByStructType(const FPulldownStructType& StructType);

		// Tears down every entry in RegisteredStructTypes. Skips the UObject-touching path when
		// the engine is shutting down (so we never call IsValid / GetNameSafe on stale UScriptStruct
		// pointers from FEngineLoop::Exit's UnloadModulesAtShutdown).
		void UnregisterAllByStructType();

	private:
		// Singleton instance. Constructed by Register, released by Unregister.
		static TUniquePtr<FPulldownStructDetailCoordinator> Instance;

		// Tracks which (StructType -> PulldownContents) pairs we have registered, so that
		// unregister can find the right StructType even when the source PulldownContents has already moved on.
		// Held as weak so a GC'd PulldownContents does not keep us in an inconsistent state.
		TMap<FPulldownStructType, TWeakObjectPtr<const UPulldownContents>> RegisteredStructTypes;

		// Delegate handles for explicit removal.
		FDelegateHandle OnPulldownContentsLoadedHandle;
		FDelegateHandle OnAllPulldownContentsLoadedHandle;
		FDelegateHandle OnPulldownStructTypeChangePendingHandle;
		FDelegateHandle OnPulldownStructTypeChangeCommittedHandle;
		FDelegateHandle OnPackageReloadedHandle;
	};
}
