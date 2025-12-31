// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "PulldownBuilderTestEditor/PulldownBuilderTestEditorSettings.h"

namespace PulldownBuilderTest
{
	class FPulldownBuilderTestEditorModule : public IModuleInterface
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.
	};

	void FPulldownBuilderTestEditorModule::StartupModule()
	{
		UPulldownBuilderTestEditorSettings::Register();
	}

	void FPulldownBuilderTestEditorModule::ShutdownModule()
	{
		UPulldownBuilderTestEditorSettings::Unregister();
	}
}

IMPLEMENT_MODULE(PulldownBuilderTest::FPulldownBuilderTestEditorModule, PulldownBuilderTestEditor);
