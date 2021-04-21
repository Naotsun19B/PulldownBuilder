// Copyright 2021 Naotsun. All Rights Reserved.

#include "Asset/AssetTypeActions_PulldownContents.h"
#include "Asset/PulldownContents.h"

#define LOCTEXT_NAMESPACE "PulldownBuilder"

FText FAssetTypeActions_PulldownContents::GetName() const
{
	return LOCTEXT("AssetName", "Pulldown Contents");
}

FColor FAssetTypeActions_PulldownContents::GetTypeColor() const
{
	return FColor(103, 206, 218);
}

UClass* FAssetTypeActions_PulldownContents::GetSupportedClass() const
{
	return UPulldownContents::StaticClass();
}

uint32 FAssetTypeActions_PulldownContents::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

bool FAssetTypeActions_PulldownContents::CanLocalize() const
{
	return false;
}

FText FAssetTypeActions_PulldownContents::GetAssetDescription(const FAssetData& AssetData) const
{
	return FAssetTypeActions_Base::GetAssetDescription(AssetData);
}

#undef LOCTEXT_NAMESPACE
