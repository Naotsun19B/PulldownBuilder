// Copyright 2021-2024 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#if !UE_5_02_OR_LATER
#include "PulldownBuilder/Assets/AssetTypeActions_PulldownContents.h"
#endif
#include "PulldownBuilder/Assets/PulldownContentsLoader.h"
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
#if !UE_5_02_OR_LATER
		// Register asset type actions.
		FAssetTypeActions_PulldownContents::Register();
#endif

		// Register the icons of this plugin.
		FPulldownBuilderStyle::Register();

		// Register message log.
		FPulldownBuilderMessageLog::Register();

		// Register pulldown contents loader.
		FPulldownContentsLoader::Register();
		
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
		
		// Unregister pulldown contents loader.
		FPulldownContentsLoader::Unregister();
		
		// Unregister message log.
		FPulldownBuilderMessageLog::Unregister();
		
		// Unregister the icons of this plugin.
		FPulldownBuilderStyle::Unregister();
		
#if !UE_5_02_OR_LATER
		// Unregister asset type actions.
		FAssetTypeActions_PulldownContents::Unregister();
#endif
	}
}
	
IMPLEMENT_MODULE(PulldownBuilder::FPulldownBuilderModule, PulldownBuilder)
