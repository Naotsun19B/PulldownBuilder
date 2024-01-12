// Copyright 2021-2024 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

namespace PulldownBuilder
{
	class FPulldownStructNodesModule : public IModuleInterface
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.
	};

	void FPulldownStructNodesModule::StartupModule()
	{
	}

	void FPulldownStructNodesModule::ShutdownModule()
	{
	}
}
	
IMPLEMENT_MODULE(PulldownBuilder::FPulldownStructNodesModule, PulldownStructNodes)
