// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/CustomPropertyTypeLayoutRegistry.h"
#include "PulldownBuilder/Assets/PulldownContentsDelegates.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IPropertyTypeCustomization.h"

namespace PulldownBuilder
{
	namespace PropertyEditorModule
	{
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
		auto& PropertyEditorModule = PropertyEditorModule::Get();
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout(*PropertyTypeName);
	}

	const FString& ICustomPropertyTypeLayoutRegistry::GetPropertyTypeName() const
	{
		return PropertyTypeName;
	}

	void FCustomizationModuleChangedNotificator::Register()
	{
		OnDetailCustomizationRegisteredHandle = FPulldownContentsDelegates::OnDetailCustomizationRegistered.AddStatic(&FCustomizationModuleChangedNotificator::NotifyCustomizationModuleChanged);
		OnDetailCustomizationUnregisteredHandle = FPulldownContentsDelegates::OnDetailCustomizationUnregistered.AddStatic(&FCustomizationModuleChangedNotificator::NotifyCustomizationModuleChanged);
	}

	void FCustomizationModuleChangedNotificator::Unregister()
	{
		FPulldownContentsDelegates::OnDetailCustomizationUnregistered.Remove(OnDetailCustomizationUnregisteredHandle);
		FPulldownContentsDelegates::OnDetailCustomizationRegistered.Remove(OnDetailCustomizationRegisteredHandle);
	}

	void FCustomizationModuleChangedNotificator::NotifyCustomizationModuleChanged(const UPulldownContents* ModifiedPulldownContents)
	{
		auto& PropertyEditorModule = PropertyEditorModule::Get();
		PropertyEditorModule.NotifyCustomizationModuleChanged();
	}

	FDelegateHandle FCustomizationModuleChangedNotificator::OnDetailCustomizationRegisteredHandle;
	FDelegateHandle FCustomizationModuleChangedNotificator::OnDetailCustomizationUnregisteredHandle;
}
