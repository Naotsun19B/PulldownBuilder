// Copyright 2021 Naotsun. All Rights Reserved.

#include "Assets/PulldownContentsFactory.h"
#include "Assets/PulldownContents.h"

UPulldownContentsFactory::UPulldownContentsFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	SupportedClass = UPulldownContents::StaticClass();
}

bool UPulldownContentsFactory::DoesSupportClass(UClass* Class)
{
	return (Class == UPulldownContents::StaticClass());
}

UClass* UPulldownContentsFactory::ResolveSupportedClass()
{
	return UPulldownContents::StaticClass();
}

UObject* UPulldownContentsFactory::FactoryCreateNew(
	UClass* InClass,
	UObject* InParent,
	FName InName,
	EObjectFlags Flags,
	UObject* Context,
	FFeedbackContext* Warn
)
{
	return NewObject<UPulldownContents>(InParent, InClass, InName, Flags);
}
