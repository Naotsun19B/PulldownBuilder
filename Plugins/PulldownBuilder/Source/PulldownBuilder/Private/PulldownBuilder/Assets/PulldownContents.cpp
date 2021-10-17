// Copyright 2021 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "PulldownBuilder/DetailCustomizations/PulldownStructDetail.h"
#include "PulldownBuilder/Structs/PulldownRow.h"

namespace PulldownContentsDefine
{
	// Define tags for the information that is displayed when you hover over an asset in the Content Browser.
	static const FName RegisteredStructTypeTag = TEXT("RegisteredStructType");
	static const FName GeneratorClassTag = TEXT("GeneratorClass");
	static const FName SourceAssetTag = TEXT("SourceAsset");
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
	OutTags.Add(FAssetRegistryTag(
		PulldownContentsDefine::GeneratorClassTag,
		GetPulldownListGeneratorClassName(),
		FAssetRegistryTag::TT_Alphabetical
	));

	// Added the name of the underlying asset of the PulldownListGenerator to AssetRegistryTags.
	FName SourceAssetName = NAME_None;
	if (IsValid(PulldownListGenerator))
	{
		if (PulldownListGenerator->HasSourceAsset())
		{
			SourceAssetName = *PulldownListGenerator->GetSourceAssetName();
		}
	}
	OutTags.Add(FAssetRegistryTag(
		PulldownContentsDefine::SourceAssetTag,
		SourceAssetName.ToString(),
		FAssetRegistryTag::TT_Alphabetical
	));
}

TArray<TSharedPtr<FPulldownRow>> UPulldownContents::GetPulldownRows() const
{
	TArray<TSharedPtr<FPulldownRow>> PulldownRows;
	
	if (IsValid(PulldownListGenerator))
	{
		PulldownRows = PulldownListGenerator->GetPulldownRows();
	}

	// Be sure to put "None" at the beginning because it may not be selected or the list may be empty.
	PulldownRows.Insert(MakeShared<FPulldownRow>(), 0);

	return PulldownRows;
}

FString UPulldownContents::GetPulldownListGeneratorClassName() const
{
	FName GeneratorClassName = NAME_None;
	if (IsValid(PulldownListGenerator))
	{
		if (const UClass* Class = PulldownListGenerator->GetClass())
		{
			GeneratorClassName = Class->GetFName();
		}
	}

	return GeneratorClassName.ToString();
}

FString UPulldownContents::GetTooltip() const
{
	FString Tooltip = FString::Printf(
		TEXT("%s : %s\n%s : %s"),
		*PulldownContentsDefine::RegisteredStructTypeTag.ToString(), *FName(*PulldownStructType).ToString(),
		*PulldownContentsDefine::GeneratorClassTag.ToString(), *GetPulldownListGeneratorClassName()
	);
	
	if (IsValid(PulldownListGenerator))
	{
		if (PulldownListGenerator->HasSourceAsset())
		{
			Tooltip += FString::Printf(
				TEXT("\n%s : %s"),
				*PulldownContentsDefine::SourceAssetTag.ToString(), *PulldownListGenerator->GetSourceAssetName()
			);
		}
	}

	return Tooltip;
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
