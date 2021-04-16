// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FPulldownBuilderEdModule : public IModuleInterface
{
public:
	// IModuleInterface interface.
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// End of IModuleInterface interface.
};

#define LOCTEXT_NAMESPACE "PulldownBuilderEd"

void FPulldownBuilderEdModule::StartupModule()
{
}

void FPulldownBuilderEdModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPulldownBuilderEdModule, PulldownBuilderEd)
