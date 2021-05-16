// Copyright 2021 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "AssetRegistryModule.h"
#include "PulldownBuilderGlobals.h"
#include "Asset/AssetTypeActions_PulldownContents.h"
#include "Asset/PulldownContents.h"
#include "CustomGraphPin/PulldownStructGraphPinFactory.h"
#include "CustomGraphPin/NativeLessPulldownStructGraphPinFactory.h"
#include "DetailCustomization/PulldownStructTypeDetail.h"
#include "DetailCustomization/PreviewPulldownStructDetail.h"
#include "DetailCustomization/NativeLessPulldownStructDetail.h"
#include "Utility/PulldownBuilderStyle.h"

DEFINE_LOG_CATEGORY(LogPulldownBuilder);

class FPulldownBuilderModule : public IModuleInterface
{
public:
	// IModuleInterface interface.
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// End of IModuleInterface interface.
};

void FPulldownBuilderModule::StartupModule()
{
	// Register asset type actions.
	FAssetTypeActions_PulldownContents::Register();

	// Register the icons of this plugin.
	FPulldownBuilderStyle::Register();
	
	// Register custom graph pin.
	FPulldownStructGraphPinFactory::Register();
	FNativeLessPulldownStructGraphPinFactory::Register();
	
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
				if (auto* PulldownContents = Cast<UPulldownContents>(AssetData.GetAsset()))
				{
					UE_LOG(LogPulldownBuilder, Log, TEXT("Loaded %s"), *PulldownContents->GetName());
				}
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
	FNativeLessPulldownStructGraphPinFactory::Unregister();
	FPulldownStructGraphPinFactory::Unregister();

	// Unregister the icons of this plugin.
	FPulldownBuilderStyle::Unregister();
	
	// Unregister asset type actions.
	FAssetTypeActions_PulldownContents::Unregister();
}
	
IMPLEMENT_MODULE(FPulldownBuilderModule, PulldownBuilder)
