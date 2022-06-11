// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "PulldownBuilder/DetailCustomizations/PulldownStructDetail.h"
#include "PulldownBuilder/Types/PulldownRow.h"

const FName UPulldownContents::RegisteredStructTypeTag = TEXT("RegisteredStructType");
const FName UPulldownContents::GeneratorClassTag = TEXT("GeneratorClass");
const FName UPulldownContents::SourceAssetTag = TEXT("SourceAsset");

bool UPulldownContents::IsEditorOnly() const
{
	return true;
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
	PulldownStructType = FPulldownStructType();
}

void UPulldownContents::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	Super::GetAssetRegistryTags(OutTags);

	// Added the name of the structure registered in this asset to AssetRegistryTags.
	OutTags.Add(FAssetRegistryTag(
		RegisteredStructTypeTag,
		FName(*PulldownStructType).ToString(),
		FAssetRegistryTag::TT_Alphabetical
	));

	// Added the PulldownListGenerator class name set for this asset to AssetRegistryTags.
	OutTags.Add(FAssetRegistryTag(
		GeneratorClassTag,
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
		SourceAssetTag,
		SourceAssetName.ToString(),
		FAssetRegistryTag::TT_Alphabetical
	));
}

const FPulldownStructType& UPulldownContents::GetPulldownStructType() const
{
	return PulldownStructType;
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
		*RegisteredStructTypeTag.ToString(), *FName(*PulldownStructType).ToString(),
		*GeneratorClassTag.ToString(), *GetPulldownListGeneratorClassName()
	);
	
	if (IsValid(PulldownListGenerator))
	{
		if (PulldownListGenerator->HasSourceAsset())
		{
			Tooltip += FString::Printf(
				TEXT("\n%s : %s"),
				*SourceAssetTag.ToString(), *PulldownListGenerator->GetSourceAssetName()
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

	PulldownBuilder::FPulldownStructDetail::Register(PulldownStructType);
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

	PulldownBuilder::FPulldownStructDetail::Unregister(PulldownStructType);
	UE_LOG(LogPulldownBuilder, Log, TEXT("[%s] %s has been unregistered from detail customization."), *GetName(), *FName(*PulldownStructType).ToString());
}
