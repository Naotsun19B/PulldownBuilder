// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/CustomPropertyTypeLayoutRegistry.h"
#include "PulldownBuilder/Assets/PulldownContentsDelegates.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IPropertyTypeCustomization.h"
#include "Misc/CoreDelegates.h"

namespace PulldownBuilder
{
	namespace PropertyEditorModule
	{
		// Returns whether the PropertyEditor module is currently loaded.
		// Use this before Get() to avoid LoadModuleChecked crashes during program shutdown.
		bool IsLoaded()
		{
			return FModuleManager::Get().IsModuleLoaded(TEXT("PropertyEditor"));
		}

		FPropertyEditorModule& Get()
		{
			return FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		}
	}

	ICustomPropertyTypeLayoutRegistry::ICustomPropertyTypeLayoutRegistry(const FString& InPropertyTypeName)
		: PropertyTypeName(InPropertyTypeName)
	{
		FPropertyEditorModule& PropertyEditorModule = PropertyEditorModule::Get();
		PropertyEditorModule.RegisterCustomPropertyTypeLayout(
			*PropertyTypeName,
			FOnGetPropertyTypeCustomizationInstance::CreateRaw(this, &ICustomPropertyTypeLayoutRegistry::MakeInstance)
		);
	}

	ICustomPropertyTypeLayoutRegistry::~ICustomPropertyTypeLayoutRegistry()
	{
		// The static Registries array in PulldownStructDetail.cpp is destroyed at program exit,
		// after FEngineLoop::Exit has already unloaded the PropertyEditor module. LoadModuleChecked
		// would crash inside FModuleManager. Skip the unregister in that window -- the PropertyEditor
		// module's own teardown already released every layout it knew about.
		if (!PropertyEditorModule::IsLoaded())
		{
			return;
		}

		FPropertyEditorModule& PropertyEditorModule = PropertyEditorModule::Get();
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout(*PropertyTypeName);
	}

	const FString& ICustomPropertyTypeLayoutRegistry::GetPropertyTypeName() const
	{
		return PropertyTypeName;
	}

	void FCustomizationModuleChangedNotificator::Register()
	{
		OnEnginePreExitHandle = FCoreDelegates::OnEnginePreExit.AddStatic(&FCustomizationModuleChangedNotificator::HandleOnEnginePreExit);
		OnAllPulldownContentsLoadedHandle = FPulldownContentsDelegates::OnAllPulldownContentsLoaded.AddStatic(&FCustomizationModuleChangedNotificator::HandleOnAllPulldownContentsLoaded);
	}

	void FCustomizationModuleChangedNotificator::HandleOnEnginePreExit()
	{
		FPulldownContentsDelegates::OnDetailCustomizationUnregistered.Remove(OnDetailCustomizationUnregisteredHandle);
		FPulldownContentsDelegates::OnDetailCustomizationRegistered.Remove(OnDetailCustomizationRegisteredHandle);
		FPulldownContentsDelegates::OnAllPulldownContentsLoaded.Remove(OnAllPulldownContentsLoadedHandle);
		FCoreDelegates::OnEnginePreExit.Remove(OnEnginePreExitHandle);
	}

	void FCustomizationModuleChangedNotificator::HandleOnAllPulldownContentsLoaded()
	{
		OnDetailCustomizationRegisteredHandle = FPulldownContentsDelegates::OnDetailCustomizationRegistered.AddStatic(&FCustomizationModuleChangedNotificator::HandleOnDetailCustomizationRegistered);
		OnDetailCustomizationUnregisteredHandle = FPulldownContentsDelegates::OnDetailCustomizationUnregistered.AddStatic(&FCustomizationModuleChangedNotificator::HandleOnDetailCustomizationUnregistered);
		NotifyCustomizationModuleChanged();
	}

	void FCustomizationModuleChangedNotificator::HandleOnDetailCustomizationRegistered(const UPulldownContents* ModifiedPulldownContents)
	{
		NotifyCustomizationModuleChanged();
	}

	void FCustomizationModuleChangedNotificator::HandleOnDetailCustomizationUnregistered(const UPulldownContents* ModifiedPulldownContents)
	{
		NotifyCustomizationModuleChanged();
	}

	void FCustomizationModuleChangedNotificator::NotifyCustomizationModuleChanged()
	{
		// Defense-in-depth: the OnEnginePreExit handler unsubscribes us before shutdown, but if
		// this notification fires from any other late path (delayed broadcast, custom delegate),
		// skip when PropertyEditor is already unloaded.
		if (!PropertyEditorModule::IsLoaded())
		{
			return;
		}

		FPropertyEditorModule& PropertyEditorModule = PropertyEditorModule::Get();
		PropertyEditorModule.NotifyCustomizationModuleChanged();
	}

	FDelegateHandle FCustomizationModuleChangedNotificator::OnEnginePreExitHandle;
	FDelegateHandle FCustomizationModuleChangedNotificator::OnAllPulldownContentsLoadedHandle;
	FDelegateHandle FCustomizationModuleChangedNotificator::OnDetailCustomizationRegisteredHandle;
	FDelegateHandle FCustomizationModuleChangedNotificator::OnDetailCustomizationUnregisteredHandle;
}
