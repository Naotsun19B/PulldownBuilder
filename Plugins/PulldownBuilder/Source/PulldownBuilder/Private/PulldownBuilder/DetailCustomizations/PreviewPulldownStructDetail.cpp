// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownBuilder/DetailCustomizations/PreviewPulldownStructDetail.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PropertyEditorModule.h"
#include "PropertyHandle.h"
#include "Modules/ModuleManager.h"

namespace PulldownBuilder
{
	void FPreviewPulldownStructDetail::Register()
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomPropertyTypeLayout(
			*GetNameSafe(FPreviewPulldownStruct::StaticStruct()),
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FPreviewPulldownStructDetail::MakeInstance)
		);
	}

	void FPreviewPulldownStructDetail::Unregister()
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout(
			*GetNameSafe(FPreviewPulldownStruct::StaticStruct())
		);
	}

	TSharedRef<IPropertyTypeCustomization> FPreviewPulldownStructDetail::MakeInstance()
	{
		return MakeShared<FPreviewPulldownStructDetail>();
	}

	TArray<TSharedPtr<FPulldownRow>> FPreviewPulldownStructDetail::GenerateSelectableValues()
	{
		// Get the list of strings to display from PulldownContents that owns this structure.
		TArray<UObject*> OuterObjects;
		StructPropertyHandle->GetOuterObjects(OuterObjects);
		for (const auto& OuterObject : OuterObjects)
		{
			if (const auto* PulldownContents = Cast<UPulldownContents>(OuterObject))
			{
				return PulldownContents->GetPulldownRows();
			}
		}

		return FPulldownBuilderUtils::GetEmptyPulldownRows();
	}
}
