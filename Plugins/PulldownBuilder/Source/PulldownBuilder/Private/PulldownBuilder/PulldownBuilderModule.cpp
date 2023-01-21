// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "PulldownBuilder/Assets/AssetTypeActions_PulldownContents.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/CustomGraphPins/PulldownStructGraphPinFactory.h"
#include "PulldownBuilder/CustomGraphPins/NativeLessPulldownStructGraphPinFactory.h"
#include "PulldownBuilder/CustomGraphPins/GraphPinContextMenuExtender.h"
#include "PulldownBuilder/DetailCustomizations/PulldownStructTypeDetail.h"
#include "PulldownBuilder/DetailCustomizations/PreviewPulldownStructDetail.h"
#include "PulldownBuilder/DetailCustomizations/NativeLessPulldownStructDetail.h"
#include "PulldownBuilder/Utilities/PulldownBuilderAppearanceSettings.h"
#include "PulldownBuilder/Utilities/PulldownBuilderRedirectSettings.h"
#include "PulldownBuilder/Utilities/PulldownBuilderMessageLog.h"
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

		// Register message log.
		FPulldownBuilderMessageLog::Register();
		
		// Register settings.
		UPulldownBuilderAppearanceSettings::Register();
		UPulldownBuilderRedirectSettings::Register();
		
		// Register custom graph pin.
		FPulldownStructGraphPinFactory::Register();
		FNativeLessPulldownStructGraphPinFactory::Register();

		// Register a graph pin context menu extension.
		FGraphPinContextMenuExtender::Register();
		
		// Register detail customizations.
		FPulldownStructTypeDetail::Register();
		FPreviewPulldownStructDetail::Register();
		FNativeLessPulldownStructDetail::Register();

		// Load all PulldownContents in the Content Browser.
		const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		AssetRegistry.OnAssetAdded().AddLambda(
			[](const FAssetData& AssetData)
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
			}
		);

		// Unregister a graph pin context menu extension when before editor close.
		if (IsValid(GEditor))
		{
			GEditor->OnEditorClose().AddStatic(&FGraphPinContextMenuExtender::Unregister);
		}
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
		UPulldownBuilderRedirectSettings::Unregister();
		UPulldownBuilderAppearanceSettings::Unregister();

		// Unregister message log.
		FPulldownBuilderMessageLog::Unregister();
		
		// Unregister the icons of this plugin.
		FPulldownBuilderStyle::Unregister();
		
		// Unregister asset type actions.
		FAssetTypeActions_PulldownContents::Unregister();
	}
}
	
IMPLEMENT_MODULE(PulldownBuilder::FPulldownBuilderModule, PulldownBuilder)
