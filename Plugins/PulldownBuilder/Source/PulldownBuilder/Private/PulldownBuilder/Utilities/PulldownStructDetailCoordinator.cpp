// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/PulldownStructDetailCoordinator.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Assets/PulldownContentsAsyncLoader.h"
#include "PulldownBuilder/Assets/PulldownContentsDelegates.h"
#include "PulldownBuilder/DetailCustomizations/PulldownStructDetail.h"
#include "Misc/CoreMiscDefines.h"
#include "UObject/UObjectGlobals.h"

namespace PulldownBuilder
{
	namespace PulldownStructDetailCoordinatorPrivate
	{
		// Returns whether UScriptStruct pointers held in RegisteredStructTypes can be safely
		// dereferenced right now. During FEngineLoop::Exit's UnloadModulesAtShutdown the UObject
		// hash table is already torn down, so IsValid / GetNameSafe would crash inside
		// FUObjectArray::IndexToObject.
		bool CanSafelyAccessUObjects()
		{
			return (UObjectInitialized() && !IsEngineExitRequested());
		}
	}

	void FPulldownStructDetailCoordinator::Register()
	{
		if (Instance.IsValid())
		{
			return;
		}

		Instance = TUniquePtr<FPulldownStructDetailCoordinator>(new FPulldownStructDetailCoordinator());
		Instance->Bind();
	}

	void FPulldownStructDetailCoordinator::Unregister()
	{
		if (!Instance.IsValid())
		{
			return;
		}

		Instance->Unbind();

		// Drop every registration BEFORE the UniquePtr deleter runs, so that the dtor itself
		// never needs to touch UScriptStruct pointers (which may already be stale by that point).
		Instance->UnregisterAllByStructType();

		Instance.Reset();
	}

	FPulldownStructDetailCoordinator::FPulldownStructDetailCoordinator() = default;

	FPulldownStructDetailCoordinator::~FPulldownStructDetailCoordinator()
	{
		// Defensive: if Unregister was bypassed, still tear down every customization we owned.
		// UnregisterAllByStructType internally guards UObject access against shutdown.
		UnregisterAllByStructType();
	}

	void FPulldownStructDetailCoordinator::Bind()
	{
		OnPulldownContentsLoadedHandle = FPulldownContentsDelegates::OnPulldownContentsLoaded.AddRaw(
			this, &FPulldownStructDetailCoordinator::HandleOnPulldownContentsLoaded
		);
		OnAllPulldownContentsLoadedHandle = FPulldownContentsDelegates::OnAllPulldownContentsLoaded.AddRaw(
			this, &FPulldownStructDetailCoordinator::HandleOnAllPulldownContentsLoaded
		);
		OnPulldownStructTypeChangePendingHandle = FPulldownContentsDelegates::OnPulldownStructTypeChangePending.AddRaw(
			this, &FPulldownStructDetailCoordinator::HandleOnPulldownStructTypeChangePending
		);
		OnPulldownStructTypeChangeCommittedHandle = FPulldownContentsDelegates::OnPulldownStructTypeChangeCommitted.AddRaw(
			this, &FPulldownStructDetailCoordinator::HandleOnPulldownStructTypeChangeCommitted
		);
		OnPackageReloadedHandle = FCoreUObjectDelegates::OnPackageReloaded.AddRaw(
			this, &FPulldownStructDetailCoordinator::HandleOnPackageReloaded
		);

		// Catch up: if any PulldownContents finished loading before this coordinator started listening,
		// register them now so the customization works on first details-panel open.
		if (FPulldownContentsAsyncLoader::IsLoadedAllPulldownContents())
		{
			HandleOnAllPulldownContentsLoaded();
		}
	}

	void FPulldownStructDetailCoordinator::Unbind()
	{
		FCoreUObjectDelegates::OnPackageReloaded.Remove(OnPackageReloadedHandle);
		FPulldownContentsDelegates::OnPulldownStructTypeChangeCommitted.Remove(OnPulldownStructTypeChangeCommittedHandle);
		FPulldownContentsDelegates::OnPulldownStructTypeChangePending.Remove(OnPulldownStructTypeChangePendingHandle);
		FPulldownContentsDelegates::OnAllPulldownContentsLoaded.Remove(OnAllPulldownContentsLoadedHandle);
		FPulldownContentsDelegates::OnPulldownContentsLoaded.Remove(OnPulldownContentsLoadedHandle);
	}

	void FPulldownStructDetailCoordinator::HandleOnPulldownContentsLoaded(const UPulldownContents* LoadedPulldownContents)
	{
		RegisterFor(LoadedPulldownContents);
	}

	void FPulldownStructDetailCoordinator::HandleOnAllPulldownContentsLoaded()
	{
		// Catch up any PulldownContents that loaded before this coordinator was registered to listen.
		FPulldownContentsAsyncLoader::EnumeratePulldownContents(
			[this](UPulldownContents& PulldownContents) -> bool
			{
				RegisterFor(&PulldownContents);
				return true;
			}
		);
	}

	void FPulldownStructDetailCoordinator::HandleOnPulldownStructTypeChangePending(UPulldownContents* PulldownContentsAboutToChange)
	{
		// The PulldownContents still reports its *old* StructType here, so we can unregister by querying it.
		UnregisterFor(PulldownContentsAboutToChange);
	}

	void FPulldownStructDetailCoordinator::HandleOnPulldownStructTypeChangeCommitted(UPulldownContents* ChangedPulldownContents)
	{
		// Now the PulldownContents reports its *new* StructType.
		RegisterFor(ChangedPulldownContents);
	}

	void FPulldownStructDetailCoordinator::HandleOnPackageReloaded(EPackageReloadPhase ReloadPhase, FPackageReloadedEvent* ReloadedEvent)
	{
		if (ReloadPhase != EPackageReloadPhase::PostBatchPostGC)
		{
			return;
		}

		// Drop every existing registration (the underlying UScriptStruct pointers may have been swapped by reload).
		TArray<FPulldownStructType> StructTypesToRefresh;
		RegisteredStructTypes.GenerateKeyArray(StructTypesToRefresh);
		for (const FPulldownStructType& StructType : StructTypesToRefresh)
		{
			UnregisterByStructType(StructType);
		}

		// Re-register from the live PulldownContents pool.
		FPulldownContentsAsyncLoader::EnumeratePulldownContents(
			[this](UPulldownContents& PulldownContents) -> bool
			{
				RegisterFor(&PulldownContents);
				return true;
			}
		);
	}

	void FPulldownStructDetailCoordinator::RegisterFor(const UPulldownContents* PulldownContents)
	{
		if (!::IsValid(PulldownContents))
		{
			return;
		}

		// Skip Class Default Objects -- they exist for reflection, not as authored assets.
		if (PulldownContents->HasAnyFlags(RF_ClassDefaultObject))
		{
			return;
		}

		const FPulldownStructType& StructType = PulldownContents->GetPulldownStructType();
		if (!StructType.IsValid())
		{
			return;
		}

		// Re-registration for the same struct type is a no-op (FCustomPropertyTypeLayoutRegistries::Register
		// asserts on duplicates), so we early out if this struct is already tracked.
		if (RegisteredStructTypes.Contains(StructType))
		{
			return;
		}

		FPulldownStructDetail::Register(StructType);
		RegisteredStructTypes.Add(StructType, PulldownContents);

		FPulldownContentsDelegates::OnDetailCustomizationRegistered.Broadcast(PulldownContents);
	}

	void FPulldownStructDetailCoordinator::UnregisterFor(const UPulldownContents* PulldownContents)
	{
		if (!::IsValid(PulldownContents))
		{
			return;
		}

		if (PulldownContents->HasAnyFlags(RF_ClassDefaultObject))
		{
			return;
		}

		const FPulldownStructType& StructType = PulldownContents->GetPulldownStructType();
		if (!StructType.IsValid())
		{
			return;
		}

		if (!RegisteredStructTypes.Contains(StructType))
		{
			return;
		}

		FPulldownStructDetail::Unregister(StructType);
		RegisteredStructTypes.Remove(StructType);

		FPulldownContentsDelegates::OnDetailCustomizationUnregistered.Broadcast(PulldownContents);
	}

	void FPulldownStructDetailCoordinator::UnregisterByStructType(const FPulldownStructType& StructType)
	{
		if (!StructType.IsValid())
		{
			RegisteredStructTypes.Remove(StructType);
			return;
		}

		const TWeakObjectPtr<const UPulldownContents>* OwnerPtr = RegisteredStructTypes.Find(StructType);
		const UPulldownContents* Owner = (OwnerPtr != nullptr ? OwnerPtr->Get() : nullptr);

		FPulldownStructDetail::Unregister(StructType);
		RegisteredStructTypes.Remove(StructType);

		if (Owner != nullptr)
		{
			FPulldownContentsDelegates::OnDetailCustomizationUnregistered.Broadcast(Owner);
		}
	}

	void FPulldownStructDetailCoordinator::UnregisterAllByStructType()
	{
		if (RegisteredStructTypes.Num() == 0)
		{
			return;
		}

		// During engine shutdown the UObject hash table is already gone, so any IsValid /
		// GetNameSafe on a UScriptStruct pointer crashes. The PropertyEditor module is
		// tearing down too, so the only safe action is to drop the local map.
		if (!PulldownStructDetailCoordinatorPrivate::CanSafelyAccessUObjects())
		{
			RegisteredStructTypes.Empty();
			return;
		}

		TArray<FPulldownStructType> StructTypesToUnregister;
		RegisteredStructTypes.GenerateKeyArray(StructTypesToUnregister);
		for (const FPulldownStructType& StructType : StructTypesToUnregister)
		{
			UnregisterByStructType(StructType);
		}
	}

	TUniquePtr<FPulldownStructDetailCoordinator> FPulldownStructDetailCoordinator::Instance;
}
