// Copyright 2021 Naotsun. All Rights Reserved.

#include "Asset/PulldownContentsFactory.h"
#include "Asset/PulldownContents.h"

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
	return CastChecked<UPulldownContents>(StaticConstructObject_Internal(InClass, InParent, InName, Flags));
}
