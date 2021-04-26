// Copyright 2021 Naotsun. All Rights Reserved.

#include "Common/PulldownBuilderUtils.h"
#include "PulldownStructBase.h"
#include "Asset/PulldownContents.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "EdGraphSchema_K2.h"

bool FPulldownBuilderUtils::IsPulldownStruct(const UScriptStruct* InStruct)
{
	// Checks if the specified structure is not temporary,
	// inherits from FPulldownStructBase, and can be used in Blueprint.
	bool bValidStruct = false;
	bool bBasedOnPulldownStructBase = false;
	bool bIsBlueprintType = false;
	
	UScriptStruct* PulldownStruct = FPulldownStructBase::StaticStruct();
	if (IsValid(PulldownStruct) && IsValid(InStruct))
	{
		bValidStruct = (InStruct->GetOutermost() != GetTransientPackage());
		bBasedOnPulldownStructBase = (InStruct->IsChildOf(PulldownStruct) && (InStruct != PulldownStruct));
		// Note: The return value of "UEdGraphSchema_K2::IsAllowableBlueprintVariableType" is true
		// because the structure that inherits any of the structures will be automatically added
		// "BlueprintType" unless "NotBlueprintType" and "BlueprintInternalUseOnly" is added in
		// the USTRUCT of the child structure.
		bIsBlueprintType = UEdGraphSchema_K2::IsAllowableBlueprintVariableType(InStruct);
	}

	return (bValidStruct && bBasedOnPulldownStructBase && bIsBlueprintType);
}

void FPulldownBuilderUtils::EnumeratePulldownContents(const TFunction<bool(UPulldownContents*)>& Callback)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.ClassNames.Add(UPulldownContents::StaticClass()->GetFName());
	Filter.bRecursivePaths = true;
	Filter.bRecursiveClasses = true;

	AssetRegistry.EnumerateAssets(Filter, [&](const FAssetData& AssetData) -> bool
    {
        if (auto* PulldownContents = Cast<UPulldownContents>(AssetData.GetAsset()))
        {
            return Callback(PulldownContents);
        }
		
        return false;
    });
}

TArray<UPulldownContents*> FPulldownBuilderUtils::GetAllPulldownContents()
{
	TArray<UPulldownContents*> PulldownContentsList;
	EnumeratePulldownContents([&](UPulldownContents* PulldownContents) -> bool
	{
		PulldownContentsList.Add(PulldownContents);
		return true;
	});
	
	return PulldownContentsList;
}

UPulldownContents* FPulldownBuilderUtils::FindPulldownContentsByStruct(const UScriptStruct* InStruct)
{
	UPulldownContents* FoundItem = nullptr;
	EnumeratePulldownContents([&](UPulldownContents* PulldownContents) -> bool
    {
        if (InStruct == PulldownContents->GetPulldownStructType().SelectedStruct)
        {
        	FoundItem = PulldownContents;
            return false;
        }
		
        return true;
    });

	return FoundItem;
}

TArray<TSharedPtr<FString>> FPulldownBuilderUtils::GetDisplayStringsFromStruct(const UScriptStruct* InStruct)
{
	TArray<TSharedPtr<FString>> DisplayStrings;
	if (UPulldownContents* FoundItem = FindPulldownContentsByStruct(InStruct))
	{
		DisplayStrings = FoundItem->GetDisplayStrings();
	}

	return DisplayStrings;
}

bool FPulldownBuilderUtils::IsRegisteredPulldownStruct(const UScriptStruct* InStruct)
{
	bool bIsRegistered = false;
	EnumeratePulldownContents([&](UPulldownContents* PulldownContents) -> bool
    {
		if (InStruct == PulldownContents->GetPulldownStructType().SelectedStruct)
		{
			bIsRegistered = true;
			return false;
		}
		
        return true;
    });
    
	return bIsRegistered;
}
