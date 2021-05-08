﻿// Copyright 2021 Naotsun. All Rights Reserved.

#include "Asset/PulldownContents.h"
#include "ListGenerator/PulldownListGeneratorBase.h"
#include "DetailCustomization/PulldownStructDetail.h"

namespace PulldownContentsDefine
{
	static const FName RegisteredStructTypeTag = TEXT("RegisteredStructType");
	static const FName GeneratorClassTag = TEXT("GeneratorClass");
}

void UPulldownContents::PostLoad()
{
	Super::PostLoad();
	
	RegisterDetailCustomization();
}

#if BEFORE_UE_4_24
void UPulldownContents::PreEditChange(UProperty* PropertyAboutToChange)
#else
void UPulldownContents::PreEditChange(FProperty* PropertyAboutToChange)
#endif
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

void UPulldownContents::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	Super::GetAssetRegistryTags(OutTags);

	// Added the name of the structure registered in this asset to AssetRegistryTags.
	OutTags.Add(FAssetRegistryTag(
		PulldownContentsDefine::RegisteredStructTypeTag,
		FName(*PulldownStructType).ToString(),
		FAssetRegistryTag::TT_Alphabetical
	));

	// Added the PulldownListGenerator class name set for this asset to AssetRegistryTags.
	FName GeneratorClassName = NAME_None;
	if (IsValid(PulldownListGenerator))
	{
		if (UClass* Class = PulldownListGenerator->GetClass())
		{
			GeneratorClassName = Class->GetFName();
		}
	}
	OutTags.Add(FAssetRegistryTag(
		PulldownContentsDefine::GeneratorClassTag,
		GeneratorClassName.ToString(),
		FAssetRegistryTag::TT_Alphabetical
	));
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
