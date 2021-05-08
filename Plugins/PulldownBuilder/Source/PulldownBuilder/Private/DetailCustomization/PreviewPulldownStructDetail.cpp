﻿// Copyright 2021 Naotsun. All Rights Reserved.

#include "DetailCustomization/PreviewPulldownStructDetail.h"
#include "PulldownBuilderGlobals.h"
#include "Asset/PulldownContents.h"
#include "Struct/PreviewPulldownStruct.h"
#include "Utility/PulldownBuilderUtils.h"
#include "PropertyEditorModule.h"
#include "PropertyHandle.h"
#include "ModuleManager.h"

void FPreviewPulldownStructDetail::Register()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout(
        FPreviewPulldownStruct::StaticStruct()->GetFName(),
        FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FPreviewPulldownStructDetail::MakeInstance)
    );
}

void FPreviewPulldownStructDetail::Unregister()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomPropertyTypeLayout(
		FPreviewPulldownStruct::StaticStruct()->GetFName()
    );
}

TSharedRef<IPropertyTypeCustomization> FPreviewPulldownStructDetail::MakeInstance()
{
	return MakeShared<FPreviewPulldownStructDetail>();
}

TArray<TSharedPtr<FString>> FPreviewPulldownStructDetail::GenerateSelectableValues()
{
	// Get the list of strings to display from PulldownContents that owns this structure.
	TArray<UObject*> OuterObjects;
	StructPropertyHandle->GetOuterObjects(OuterObjects);
	for (const auto& OuterObject : OuterObjects)
	{
		if (auto* PulldownContents = Cast<UPulldownContents>(OuterObject))
		{
			return PulldownContents->GetDisplayStrings();
		}
	}

	return FPulldownBuilderUtils::GetEmptyDisplayStrings();
}
