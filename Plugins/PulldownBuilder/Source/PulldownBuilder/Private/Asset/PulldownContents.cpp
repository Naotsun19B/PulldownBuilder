// Copyright 2021 Naotsun. All Rights Reserved.

#include "Asset/PulldownContents.h"
#include "PulldownBuilderGlobals.h"
#include "ListGenerator/PulldownListGeneratorBase.h"

void UPulldownContents::PostLoad()
{
	Super::PostLoad();
	
	RegisterDetailCustomization();
}

void UPulldownContents::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

	if (PropertyAboutToChange == nullptr)
	{
		return;
	}

	if (PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(UPulldownContents, PulldownStructType) ||
		PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(FPulldownStructType, SelectedStruct))
	{
		UnregisterDetailCustomization();
	}
}

void UPulldownContents::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UPulldownContents, PulldownStructType))
	{
		RegisterDetailCustomization();
	}
}

void UPulldownContents::BeginDestroy()
{
	UnregisterDetailCustomization();
	
	Super::BeginDestroy();
}

TArray<TSharedPtr<FString>> UPulldownContents::GetDisplayStrings() const
{
	TArray<TSharedPtr<FString>> DisplayStrings;
	
	if (IsValid(PulldownListGenerator))
	{
		DisplayStrings = PulldownListGenerator->GetDisplayStrings();
	}

	return DisplayStrings;
}

void UPulldownContents::RegisterDetailCustomization()
{
	// The default object does not perform registration processing.
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

	// If the Pulldown Struct Type is empty, the process will be skipped.
	if (PulldownStructType.IsEmpty())
	{
		return;
	}

	const FName& StructName = *PulldownStructType;
	//FPulldownStructDetail::Register(StructName);
	UE_LOG(LogPulldownBuilder, Log, TEXT("[%s] %s has been registered with detail customization."), *GetName(), *StructName.ToString());
}

void UPulldownContents::UnregisterDetailCustomization()
{
	// The default object does not perform registration processing.
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

	// If the Pulldown Struct Type is empty, the process will be skipped.
	if (PulldownStructType.IsEmpty())
	{
		return;
	}

	const FName& StructName = *PulldownStructType;
	//FPulldownStructDetail::Unregister(StructName);
	UE_LOG(LogPulldownBuilder, Log, TEXT("[%s] %s has been unregistered from detail customization."), *GetName(), *StructName.ToString());
}
