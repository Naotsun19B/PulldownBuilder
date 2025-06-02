// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/DetailCustomizations/PreviewPulldownStructDetail.h"
#include "PulldownBuilder/Utilities/CustomPropertyTypeLayoutRegistry.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Types/StructContainer.h"

namespace PulldownBuilder
{
	void FPreviewPulldownStructDetail::Register()
	{
		CustomPropertyTypeLayoutRegistry = MakeUnique<TCustomPropertyTypeLayoutRegistry<FPreviewPulldownStruct, FPreviewPulldownStructDetail>>();
	}

	void FPreviewPulldownStructDetail::Unregister()
	{
		CustomPropertyTypeLayoutRegistry.Reset();
	}

	FPulldownRows FPreviewPulldownStructDetail::GenerateSelectableValues()
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

		return FPulldownRows::Empty;
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

	TUniquePtr<ICustomPropertyTypeLayoutRegistry> FPreviewPulldownStructDetail::CustomPropertyTypeLayoutRegistry;
}
