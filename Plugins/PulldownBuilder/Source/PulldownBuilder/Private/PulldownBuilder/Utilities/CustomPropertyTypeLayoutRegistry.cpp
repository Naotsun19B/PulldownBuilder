// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/CustomPropertyTypeLayoutRegistry.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IPropertyTypeCustomization.h"

namespace PulldownBuilder
{
	namespace CustomPropertyTypeLayoutRegistry
	{
		FPropertyEditorModule& GetPropertyEditorModule()
		{
			return FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		}
	}
	
	ICustomPropertyTypeLayoutRegistry::ICustomPropertyTypeLayoutRegistry(const FString& InPropertyTypeName)
		: PropertyTypeName(InPropertyTypeName)
	{
		FPropertyEditorModule& PropertyEditorModule = CustomPropertyTypeLayoutRegistry::GetPropertyEditorModule();
		PropertyEditorModule.RegisterCustomPropertyTypeLayout(
			*PropertyTypeName,
			FOnGetPropertyTypeCustomizationInstance::CreateRaw(this, &ICustomPropertyTypeLayoutRegistry::MakeInstance)
		);
	}

	ICustomPropertyTypeLayoutRegistry::~ICustomPropertyTypeLayoutRegistry()
	{
		auto& PropertyEditorModule = CustomPropertyTypeLayoutRegistry::GetPropertyEditorModule();
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout(*PropertyTypeName);
	}

	const FString& ICustomPropertyTypeLayoutRegistry::GetPropertyTypeName() const
	{
		return PropertyTypeName;
	}
}
