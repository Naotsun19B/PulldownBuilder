// Copyright 2021 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FPulldownStructModule : public IModuleInterface
{
public:
	// IModuleInterface interface.
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
	// End of IModuleInterface interface.
};
	
IMPLEMENT_MODULE(FPulldownStructModule, PulldownStruct)
