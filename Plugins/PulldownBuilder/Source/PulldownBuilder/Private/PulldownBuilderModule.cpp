// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "AssetToolsModule.h"
#include "AssetRegistryModule.h"
#include "PulldownBuilderGlobals.h"
#include "Asset/AssetTypeActions_PulldownContents.h"
#include "Asset/PulldownContents.h"
#include "CustomGraphPin/PulldownStructGraphPinFactory.h"
#include "DetailCustomization/NativeLessPulldownStructDetail.h"
#include "DetailCustomization/PulldownStructTypeDetail.h"
#include "DetailCustomization/PreviewPulldownStructDetail.h"
#include "Utility/PulldownBuilderStyle.h"

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

	// Register the icons of this plugin.
	FPulldownBuilderStyle::Register();
	
	// Register custom graph pin.
	FPulldownStructGraphPinFactory::Register();
	
	// Register detail customizations.
	FPulldownStructTypeDetail::Register();
	FPreviewPulldownStructDetail::Register();
	FNativeLessPulldownStructDetail::Register();

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
	FNativeLessPulldownStructDetail::Unregister();
	FPreviewPulldownStructDetail::Unregister();
	FPulldownStructTypeDetail::Unregister();

	// Unregister custom graph pin.
	FPulldownStructGraphPinFactory::Unregister();

	// Unregister the icons of this plugin.
	FPulldownBuilderStyle::Unregister();
	
	// Unregister asset type actions.
	if (AssetTypeActions.IsValid() && FAssetToolsModule::IsModuleLoaded())
	{
		FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(AssetTypeActions.ToSharedRef());
		AssetTypeActions.Reset();
	}
}
	
IMPLEMENT_MODULE(FPulldownBuilderModule, PulldownBuilder)
