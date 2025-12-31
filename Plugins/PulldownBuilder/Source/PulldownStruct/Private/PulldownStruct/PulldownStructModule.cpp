// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

DEFINE_LOG_CATEGORY(LogPulldownBuilder);

namespace PulldownBuilder
{
	class FPulldownStructModule : public IModuleInterface
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.
	};

	void FPulldownStructModule::StartupModule()
	{
	}

	void FPulldownStructModule::ShutdownModule()
	{
	}
}
	
IMPLEMENT_MODULE(PulldownBuilder::FPulldownStructModule, PulldownStruct)
