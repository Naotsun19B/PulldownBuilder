// Copyright 2021 Naotsun. All Rights Reserved.

#include "Common/PulldownBuilderUtils.h"
#include "PulldownStructBase.h"
#include "Asset/PulldownContents.h"
#include "AssetRegistry/AssetRegistryModule.h"

bool FPulldownBuilderUtils::IsPulldownStruct(const UScriptStruct* InStruct)
{
	bool bValidStruct = false;
	bool bBasedOnPulldownStructBase = false;
	if (UScriptStruct* PulldownStruct = FPulldownStructBase::StaticStruct())
	{
		bValidStruct = (InStruct->GetOutermost() != GetTransientPackage());
		bBasedOnPulldownStructBase = PulldownStruct && InStruct->IsChildOf(PulldownStruct) && (InStruct != PulldownStruct);
	}

	return (bValidStruct && bBasedOnPulldownStructBase);
}

#pragma optimize("", off)
void FPulldownBuilderUtils::EnumeratePulldownContents(const TFunction<bool(UPulldownContents*)>& Callback)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.PackagePaths.Add(TEXT("/Game/"));
	Filter.ClassNames.Add(UPulldownContents::StaticClass()->GetFName());
	Filter.bRecursivePaths = true;
	Filter.bRecursiveClasses = true;
	Filter.bIncludeOnlyOnDiskAssets = false;

	TArray<FAssetData> AssetData;
	AssetRegistry.GetAllAssets(AssetData);
	
	AssetRegistry.EnumerateAssets(Filter, [&](const FAssetData& AssetData) -> bool
    {
        if (auto* PulldownContents = Cast<UPulldownContents>(AssetData.GetAsset()))
        {
            return Callback(PulldownContents);
        }
		
        return false;
    });
}
#pragma optimize("", on)

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

bool FPulldownBuilderUtils::IsRegisteredPulldownStruct(const UScriptStruct* InStruct)
{
	bool bIsRegistered = false;
	EnumeratePulldownContents([&](UPulldownContents* PulldownContents) -> bool
    {
		const FString& LValue = (InStruct != nullptr ? InStruct->GetName() : TEXT("nullptr"));
		const FString& RValue = (PulldownContents->GetPulldownStructType().SelectedStruct != nullptr ? PulldownContents->GetPulldownStructType().SelectedStruct->GetName() : TEXT("nullptr"));
		UE_LOG(LogTemp, Error, TEXT("%s == %s"), *LValue, *RValue);
		if (InStruct == PulldownContents->GetPulldownStructType().SelectedStruct)
		{
			bIsRegistered = true;
			return false;
		}
        return true;
    });
    
	return bIsRegistered;
}
