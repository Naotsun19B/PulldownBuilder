// Copyright 2021 Naotsun. All Rights Reserved.

#include "Asset/PulldownContents.h"
#include "PulldownBuilderGlobals.h"
#include "ListGenerator/PulldownListGeneratorBase.h"
#include "DetailCustomization/PulldownStructDetail.h"

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

void UPulldownContents::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);

	// Prevent the same structure from being registered when duplicated.
	PulldownStructType = nullptr;
}

TArray<TSharedPtr<FString>> UPulldownContents::GetDisplayStrings() const
{
	TArray<TSharedPtr<FString>> DisplayStrings;
	
	if (IsValid(PulldownListGenerator))
	{
		DisplayStrings = PulldownListGenerator->GetDisplayStrings();
	}

	// Be sure to put "None" at the beginning because it may not be selected or the list may be empty.
	DisplayStrings.Insert(MakeShared<FString>(FName(NAME_None).ToString()), 0);

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

	FPulldownStructDetail::Register(PulldownStructType);
	UE_LOG(LogPulldownBuilder, Log, TEXT("[%s] %s has been registered with detail customization."), *GetName(), *FName(*PulldownStructType).ToString());
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

	FPulldownStructDetail::Unregister(PulldownStructType);
	UE_LOG(LogPulldownBuilder, Log, TEXT("[%s] %s has been unregistered from detail customization."), *GetName(), *FName(*PulldownStructType).ToString());
}
