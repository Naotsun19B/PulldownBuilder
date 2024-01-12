﻿// Copyright 2021-2024 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Assets/AssetDefinition_PulldownContents.h"
#include "PulldownBuilder/Assets/PulldownContents.h"

#define LOCTEXT_NAMESPACE "AssetDefinition_PulldownContents"

namespace PulldownBuilder
{
#if UE_5_02_OR_LATER
	FText UAssetDefinition_PulldownContents::GetAssetDisplayName() const
	{
		return LOCTEXT("AssetName", "Pulldown Contents");
	}

	FLinearColor UAssetDefinition_PulldownContents::GetAssetColor() const
	{
		return FColor(103, 206, 218);
	}

	TSoftClassPtr<UObject> UAssetDefinition_PulldownContents::GetAssetClass() const
	{
		return UPulldownContents::StaticClass();
	}

	TConstArrayView<FAssetCategoryPath> UAssetDefinition_PulldownContents::GetAssetCategories() const
	{
		static const auto Categories = { EAssetCategoryPaths::Misc };
		return Categories;
	}

	FAssetSupportResponse UAssetDefinition_PulldownContents::CanLocalize(const FAssetData& InAsset) const
	{
		return FAssetSupportResponse::NotSupported();
	}

	IMPLEMENT_INTRINSIC_CLASS(
		UAssetDefinition_PulldownContents,
		PULLDOWNBUILDER_API,
		UAssetDefinitionDefault,
		UNREALED_API,
		TEXT("/Script/PulldownBuilder"),
		{
		}
	)
#endif
}

#undef LOCTEXT_NAMESPACE
