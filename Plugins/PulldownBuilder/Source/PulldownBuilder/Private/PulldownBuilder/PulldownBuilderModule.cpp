// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#if !UE_5_02_OR_LATER
#include "PulldownBuilder/Assets/AssetTypeActions_PulldownContents.h"
#endif
#include "PulldownBuilder/Assets/PulldownContentsAsyncLoader.h"
#include "PulldownBuilder/CommandActions/PulldownBuilderCommands.h"
#include "PulldownBuilder/CustomGraphPins/PulldownStructGraphPinFactory.h"
#include "PulldownBuilder/CustomGraphPins/NativeLessPulldownStructGraphPinFactory.h"
#include "PulldownBuilder/CustomGraphPins/GraphPinContextMenuExtender.h"
#include "PulldownBuilder/DetailCustomizations/PulldownStructTypeDetail.h"
#include "PulldownBuilder/DetailCustomizations/PreviewPulldownStructDetail.h"
#include "PulldownBuilder/DetailCustomizations/NativeLessPulldownStructDetail.h"
#include "PulldownBuilder/RowNameUpdaters/RowNameUpdaterBase.h"
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
		// Registers asset type actions.
		FAssetTypeActions_PulldownContents::Register();
#endif

		// Registers the icons of this plugin.
		FPulldownBuilderStyle::Register();

		// Registers command actions.
		FPulldownBuilderCommands::Register();

		// Registers message log.
		FPulldownBuilderMessageLog::Register();
		
		// Registers pulldown contents async loader.
		FPulldownContentsAsyncLoader::Register();
		
		// Registers settings.
		UPulldownBuilderAppearanceSettings::Register();
		UPulldownBuilderRedirectSettings::Register();

		// Registers row name updaters.
		URowNameUpdaterBase::Register();
		
		// Registers custom graph pin.
		FPulldownStructGraphPinFactory::Register();
		FNativeLessPulldownStructGraphPinFactory::Register();

		// Registers a graph pin context menu extension.
		FGraphPinContextMenuExtender::Register();
		
		// Registers detail customizations.
		FPulldownStructTypeDetail::Register();
		FPreviewPulldownStructDetail::Register();
		FNativeLessPulldownStructDetail::Register();
	}

	void FPulldownBuilderModule::ShutdownModule()
	{
		// Unregisters detail customizations.
		FNativeLessPulldownStructDetail::Unregister();
		FPreviewPulldownStructDetail::Unregister();
		FPulldownStructTypeDetail::Unregister();
		
		// Unregisters custom graph pin.
		FNativeLessPulldownStructGraphPinFactory::Unregister();
		FPulldownStructGraphPinFactory::Unregister();

		// Unregisters row name updaters.
		URowNameUpdaterBase::Unregister();

		// Unregisters settings.
		UPulldownBuilderRedirectSettings::Unregister();
		UPulldownBuilderAppearanceSettings::Unregister();
		
		// Unregisters pulldown contents async loader.
		FPulldownContentsAsyncLoader::Unregister();
		
		// Unregisters message log.
		FPulldownBuilderMessageLog::Unregister();

		// Unregisters command actions.
		FPulldownBuilderCommands::Unregister();
		
		// Unregisters the icons of this plugin.
		FPulldownBuilderStyle::Unregister();
		
#if !UE_5_02_OR_LATER
		// Unregisters asset type actions.
		FAssetTypeActions_PulldownContents::Unregister();
#endif
	}
}
	
IMPLEMENT_MODULE(PulldownBuilder::FPulldownBuilderModule, PulldownBuilder)
