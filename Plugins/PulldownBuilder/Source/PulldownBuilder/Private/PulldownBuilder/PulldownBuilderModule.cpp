// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "AssetRegistryModule.h"
#include "PulldownBuilder/Assets/AssetTypeActions_PulldownContents.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/CustomGraphPins/PulldownStructGraphPinFactory.h"
#include "PulldownBuilder/CustomGraphPins/NativeLessPulldownStructGraphPinFactory.h"
#include "PulldownBuilder/DetailCustomizations/PulldownStructTypeDetail.h"
#include "PulldownBuilder/DetailCustomizations/PreviewPulldownStructDetail.h"
#include "PulldownBuilder/DetailCustomizations/NativeLessPulldownStructDetail.h"
#include "PulldownBuilder/Utilities/PulldownBuilderSettings.h"
#include "PulldownBuilder/Utilities/PulldownBuilderStyle.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

namespace PulldownBuilder
{
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

		// Register settings.
		UPulldownBuilderSettings::Register();
		
		// Register custom graph pin.
		FPulldownStructGraphPinFactory::Register();
		FNativeLessPulldownStructGraphPinFactory::Register();
		
		// Register detail customizations.
		FPulldownStructTypeDetail::Register();
		FPreviewPulldownStructDetail::Register();
		FNativeLessPulldownStructDetail::Register();

		// Load all PulldownContents in the Content Browser.
		const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		AssetRegistry.OnAssetAdded().AddLambda([](const FAssetData& AssetData)
		{
			if (const UClass* Class = AssetData.GetClass())
			{
				if (Class->IsChildOf<UPulldownContents>())
				{
					if (const auto* PulldownContents = Cast<UPulldownContents>(AssetData.GetAsset()))
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

		// Unregister settings.
		UPulldownBuilderSettings::Unregister();
		
		// Unregister the icons of this plugin.
		FPulldownBuilderStyle::Unregister();
		
		// Unregister asset type actions.
		FAssetTypeActions_PulldownContents::Unregister();
	}
}
	
IMPLEMENT_MODULE(PulldownBuilder::FPulldownBuilderModule, PulldownBuilder)
