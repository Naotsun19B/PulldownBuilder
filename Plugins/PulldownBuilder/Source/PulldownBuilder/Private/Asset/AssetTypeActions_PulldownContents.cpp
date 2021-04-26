// Copyright 2021 Naotsun. All Rights Reserved.

#include "Asset/AssetTypeActions_PulldownContents.h"
#include "Asset/PulldownContents.h"
#include "AssetToolsModule.h"

#define LOCTEXT_NAMESPACE "PulldownBuilder"

TSharedPtr<FAssetTypeActions_PulldownContents> FAssetTypeActions_PulldownContents::Instance = nullptr;

void FAssetTypeActions_PulldownContents::Register()
{
	Instance = MakeShared<FAssetTypeActions_PulldownContents>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(Instance.ToSharedRef());
}

void FAssetTypeActions_PulldownContents::Unregister()
{
	if (Instance.IsValid() && FAssetToolsModule::IsModuleLoaded())
	{
		FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(Instance.ToSharedRef());
		Instance.Reset();
	}
}

FAssetTypeActions_PulldownContents& FAssetTypeActions_PulldownContents::Get()
{
	check(Instance.IsValid()); // Don't call before Register is called or after Unregister is called.
	return *Instance.Get();
}

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

#undef LOCTEXT_NAMESPACE
