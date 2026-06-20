// Copyright 2021-2026 Naotsun. All Rights Reserved.

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
#include "PulldownBuilder/Utilities/CustomPropertyTypeLayoutRegistry.h"
#include "PulldownBuilder/Utilities/PulldownBuilderSettings.h"
#include "PulldownBuilder/Utilities/PulldownBuilderMessageLog.h"
#include "PulldownBuilder/Utilities/PulldownBuilderStyle.h"
#include "PulldownBuilder/Utilities/PulldownStructDetailCoordinator.h"

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
		// Initialization order is meaningful. The blocks below are grouped by responsibility and ordered
		// so that each consumer is registered AFTER everything it observes / depends on. ShutdownModule
		// runs the reverse order.
		//
		//   1. Asset registry / asset definition entry points -- so the PulldownContents asset class is
		//      recognized before the async loader starts surfacing instances of it.
		//   2. Style / commands / message log -- pure UI / diagnostics primitives with no dependencies.
		//   3. Async loader -- starts listening to the AssetRegistry. After this point, OnPulldownContentsLoaded
		//      can fire, so every listener must be ready to receive it (steps 4 onward).
		//   4. Customization-module notificator -- relays detail-customization-module changes that downstream
		//      detail customization registrations depend on.
		//   5. Settings -- registered before any module that reads them at registration time.
		//   6. Row-name updaters -- subscribe to OnPulldownRowRenamed so a delegate fire during 7+ is handled.
		//   7. Graph pin factories / context menu extensions -- need PulldownContents assets to already be
		//      discoverable in order to bind correctly.
		//   8. Detail customizations -- registered last so the PropertyEditor module sees the latest factory state.

#if !UE_5_02_OR_LATER
		// 1. Asset type actions (UE5.1 and earlier). UE5.2+ uses UAssetDefinition discovered automatically.
		FAssetTypeActions_PulldownContents::Register();
#endif

		// 2. Style / commands / message log (no deps).
		FPulldownBuilderStyle::Register();
		FPulldownBuilderCommands::Register();
		FPulldownBuilderMessageLog::Register();

		// 3. Async loader. Must come BEFORE listeners that react to OnPulldownContentsLoaded.
		FPulldownContentsAsyncLoader::Register();

		// 4. Detail-customization-module notificator.
		FCustomizationModuleChangedNotificator::Register();

		// 5. Settings.
		UPulldownBuilderSettings::Register();

		// 6. Row name updaters. Subscribes to OnPulldownRowRenamed broadcast from PulldownContents.
		URowNameUpdaterBase::Register();

		// 7. Graph pin factories and context menu extension.
		FPulldownStructGraphPinFactory::Register();
		FNativeLessPulldownStructGraphPinFactory::Register();
		FGraphPinContextMenuExtender::Register();

		// 8. Detail customizations.
		FPulldownStructTypeDetail::Register();
		FPreviewPulldownStructDetail::Register();
		FNativeLessPulldownStructDetail::Register();

		// 9. Detail-customization coordinator. Registered last because it Broadcasts via the property editor
		//    module and needs every other listener (RowNameUpdater, NativeLessPulldownStructDetail, ...)
		//    already in place. Inside Register() it also catches up on any PulldownContents that already
		//    finished loading.
		FPulldownStructDetailCoordinator::Register();
	}

	void FPulldownBuilderModule::ShutdownModule()
	{
		// Reverse of StartupModule. Each block tears down the corresponding initialization step.

		// 9. Coordinator -- stop listening first so subsequent unregistrations don't trigger needless work.
		FPulldownStructDetailCoordinator::Unregister();

		// 8. Detail customizations.
		FNativeLessPulldownStructDetail::Unregister();
		FPreviewPulldownStructDetail::Unregister();
		FPulldownStructTypeDetail::Unregister();

		// 7. Graph pin factories and context menu extension are torn down implicitly by the property editor /
		//    graph editor module on shutdown; only the factory pair has an explicit Unregister.
		FNativeLessPulldownStructGraphPinFactory::Unregister();
		FPulldownStructGraphPinFactory::Unregister();

		// 6. Row name updaters.
		URowNameUpdaterBase::Unregister();

		// 3. Async loader. Stops listening to the AssetRegistry first so no further callbacks fire while
		//    the rest of the module tears down.
		FPulldownContentsAsyncLoader::Unregister();

		// 2. Style / commands / message log.
		FPulldownBuilderMessageLog::Unregister();
		FPulldownBuilderCommands::Unregister();
		FPulldownBuilderStyle::Unregister();

#if !UE_5_02_OR_LATER
		// 1. Asset type actions.
		FAssetTypeActions_PulldownContents::Unregister();
#endif
	}
}
	
IMPLEMENT_MODULE(PulldownBuilder::FPulldownBuilderModule, PulldownBuilder)
