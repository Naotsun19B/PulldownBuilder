// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "PulldownBuilderGlobals.h"
#include "DetailCustomization/PulldownStructTypeDetail.h"

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
	// Register detail customizations.
	FPulldownStructTypeDetail::Register();
}

void FPulldownBuilderModule::ShutdownModule()
{
	// Unregister detail customizations.
	FPulldownStructTypeDetail::Unregister();
}
	
IMPLEMENT_MODULE(FPulldownBuilderModule, PulldownBuilder)
