// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "PulldownContentsFactory.generated.h"

/**
 * A factory class that creates PulldownContents assets.
 */
UCLASS(NotBlueprintable, HideCategories = "Object")
class PULLDOWNBUILDER_API UPulldownContentsFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

public:
	// UFactory interface.
	virtual bool DoesSupportClass(UClass* Class) override;
    virtual UClass* ResolveSupportedClass() override;
    virtual UObject* FactoryCreateNew(
    	UClass* InClass,
    	UObject* InParent,
    	FName InName,
    	EObjectFlags Flags,
    	UObject* Context,
    	FFeedbackContext* Warn
    ) override;
	// End of UFactory interface.
};
