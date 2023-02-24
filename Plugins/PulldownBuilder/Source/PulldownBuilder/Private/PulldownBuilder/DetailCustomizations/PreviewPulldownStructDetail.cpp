// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownBuilder/DetailCustomizations/PreviewPulldownStructDetail.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Types/StructContainer.h"
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
		// Get the list of strings to display from PulldownContents that owns this struct.
		TArray<UObject*> OuterObjects;
		StructPropertyHandle->GetOuterObjects(OuterObjects);
		for (const auto& OuterObject : OuterObjects)
		{
			if (auto* PulldownContents = Cast<UPulldownContents>(OuterObject))
			{
#if UE_4_25_OR_LATER
				if (const auto* StructProperty = CastField<FStructProperty>(StructPropertyHandle->GetProperty()))
#else
				if (const auto* StructProperty = Cast<UStructProperty>(StructPropertyHandle->GetProperty()))
#endif
				{
					void* RawData;
					const FPropertyAccess::Result Result = StructPropertyHandle->GetValueData(RawData);
					if (Result != FPropertyAccess::Success)
					{
						RawData = nullptr;
					}
					
					return PulldownContents->GetPulldownRows(
						TArray<UObject*>{ PulldownContents },
						FStructContainer(StructProperty->Struct, static_cast<uint8*>(RawData))
					);
				}
			}
		}

		return FPulldownBuilderUtils::GetEmptyPulldownRows();
	}
}
