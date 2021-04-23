﻿// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "AssetToolsModule.h"
#include "AssetRegistryModule.h"
#include "PulldownBuilderGlobals.h"
#include "Asset/AssetTypeActions_PulldownContents.h"
#include "Asset/PulldownContents.h"
#include "DetailCustomization/PulldownStructTypeDetail.h"

DEFINE_LOG_CATEGORY(LogPulldownBuilder);

class FPulldownBuilderModule : public IModuleInterface
{
public:
	// IModuleInterface interface.
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// End of IModuleInterface interface.

private:
	// An instance of a class that defines information and operations for UPulldownContents.
	TSharedPtr<FAssetTypeActions_PulldownContents> AssetTypeActions;
};

void FPulldownBuilderModule::StartupModule()
{
	// Register asset type actions.
	AssetTypeActions = MakeShared<FAssetTypeActions_PulldownContents>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(AssetTypeActions.ToSharedRef());
	
	// Register detail customizations.
	FPulldownStructTypeDetail::Register();

	// Load all PulldownContents in the Content Browser.
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	AssetRegistry.OnAssetAdded().AddLambda([](const FAssetData& AssetData)
	{
		if (UClass* Class = AssetData.GetClass())
		{
			if (Class->IsChildOf<UPulldownContents>())
			{
				AssetData.GetAsset();
			}
		}
	});
}

void FPulldownBuilderModule::ShutdownModule()
{
	// Unregister detail customizations.
	FPulldownStructTypeDetail::Unregister();

	// Unregister asset type actions.
	if (AssetTypeActions.IsValid() && FAssetToolsModule::IsModuleLoaded())
	{
		FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(AssetTypeActions.ToSharedRef());
		AssetTypeActions.Reset();
	}
}
	
IMPLEMENT_MODULE(FPulldownBuilderModule, PulldownBuilder)
