// Copyright 2021-2024 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Assets/PulldownContentsFactory.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(PulldownContentsFactory)
#endif

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
