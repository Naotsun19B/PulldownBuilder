// Copyright 2021-2024 Naotsun. All Rights Reserved.

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
		CachedPropertyTypeName = GetNameSafe(FPreviewPulldownStruct::StaticStruct());
		
		auto& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		PropertyEditorModule.RegisterCustomPropertyTypeLayout(
			*CachedPropertyTypeName,
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FPreviewPulldownStructDetail::MakeInstance)
		);
	}

	void FPreviewPulldownStructDetail::Unregister()
	{
		auto& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout(
			*CachedPropertyTypeName
		);
	}

	TSharedRef<IPropertyTypeCustomization> FPreviewPulldownStructDetail::MakeInstance()
	{
		return MakeShared<FPreviewPulldownStructDetail>();
	}

	TArray<TSharedPtr<FPulldownRow>> FPreviewPulldownStructDetail::GenerateSelectableValues()
	{
		if (auto* PulldownContents = GetRelatedPulldownContents())
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

		return FPulldownBuilderUtils::GetEmptyPulldownRows();
	}

	UPulldownContents* FPreviewPulldownStructDetail::GetRelatedPulldownContents() const
	{
		// Gets the list of strings to display from PulldownContents that owns this struct.
		TArray<UObject*> OuterObjects;
		StructPropertyHandle->GetOuterObjects(OuterObjects);
		for (const auto& OuterObject : OuterObjects)
		{
			if (auto* PulldownContents = Cast<UPulldownContents>(OuterObject))
			{
				return PulldownContents;
			}
		}

		return nullptr;
	}

	FString FPreviewPulldownStructDetail::CachedPropertyTypeName;
}
