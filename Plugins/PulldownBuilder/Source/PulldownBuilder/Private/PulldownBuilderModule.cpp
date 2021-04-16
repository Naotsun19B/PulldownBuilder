// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FPulldownBuilderModule : public IModuleInterface
{
public:
	// IModuleInterface interface.
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// End of IModuleInterface interface.
};

#define LOCTEXT_NAMESPACE "PulldownBuilder"

void FPulldownBuilderModule::StartupModule()
{
}

void FPulldownBuilderModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPulldownBuilderModule, PulldownBuilder)
