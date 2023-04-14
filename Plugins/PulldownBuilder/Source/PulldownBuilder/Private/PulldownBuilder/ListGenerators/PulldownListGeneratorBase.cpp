// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Assets/PulldownContentsLoader.h"
#include "UObject/UObjectThreadContext.h"

const FName UPulldownListGeneratorBase::FilterPulldownStructTypesName = TEXT("FilterPulldownStructTypes");

TArray<TSharedPtr<FPulldownRow>> UPulldownListGeneratorBase::GetPulldownRows(
	const TArray<UObject*>& OuterObjects,
	const FStructContainer& StructInstance
) const
{
	TArray<TSharedPtr<FPulldownRow>> PulldownRows;
	TArray<FPulldownRow> PulldownRowsFromBlueprint = GetPulldownRowsFromBlueprint(OuterObjects, StructInstance);
	for (const auto& PulldownRowFromBlueprint : PulldownRowsFromBlueprint)
	{
		PulldownRows.Add(MakeShared<FPulldownRow>(PulldownRowFromBlueprint));
	}

	return PulldownRows;
}

bool UPulldownListGeneratorBase::HasSourceAsset() const
{
	// Blueprint functions are not available during routing post load.
	if (!FUObjectThreadContext::Get().IsRoutingPostLoad)
	{
		return HasSourceAssetFromBlueprint();
	}

	return false;
}

FString UPulldownListGeneratorBase::GetSourceAssetName() const
{
	const FString& SourceAssetName = GetSourceAssetNameFromBlueprint();;
	if (SourceAssetName.IsEmpty())
	{
		unimplemented(); // Be sure to override it when using this function.
	}
	
	return SourceAssetName;
}

TArray<UScriptStruct*> UPulldownListGeneratorBase::GetFilterPulldownStructTypes() const
{
	TArray<UScriptStruct*> FilterPulldownStructTypes;

	TArray<FString> FilterPulldownStructTypeNames;
	if (const UClass* Class = GetClass())
	{
		if (Class->HasMetaData(FilterPulldownStructTypesName))
		{
			const FString MetaString = Class->GetMetaData(FilterPulldownStructTypesName);
			MetaString.ParseIntoArray(FilterPulldownStructTypeNames, TEXT(","));
		}
	}

	const TArray<FName>& FilterPulldownStructTypeNamesFromBlueprint = GetFilterPulldownStructTypesFromBlueprint();
	for (const auto& FilterPulldownStructTypeNameFromBlueprint : FilterPulldownStructTypeNamesFromBlueprint)
	{
		FilterPulldownStructTypeNames.Add(FilterPulldownStructTypeNameFromBlueprint.ToString());
	}

	FilterPulldownStructTypes.Reserve(FilterPulldownStructTypeNames.Num());
	for (auto& FilterPulldownStructTypeName : FilterPulldownStructTypeNames)
	{
		FilterPulldownStructTypeName.TrimStartAndEndInline();

		auto* FoundStruct = FindObject<UScriptStruct>(
#if UE_5_01_OR_LATER
			nullptr,
#else
			ANY_PACKAGE,
#endif
			*FilterPulldownStructTypeName
		);
		if (IsValid(FoundStruct))
		{
			FilterPulldownStructTypes.Add(FoundStruct);
		}
	}
	
	return FilterPulldownStructTypes;
}

void UPulldownListGeneratorBase::NotifyPulldownRowAdded(const FName& AddedChangeName)
{
	PulldownBuilder::FPulldownContentsLoader::OnPulldownRowAdded.Broadcast(
		GetTypedOuter<UPulldownContents>(),
		AddedChangeName
	);
}

void UPulldownListGeneratorBase::NotifyPulldownRowRemoved(const FName& RemovedChangeName)
{
	PulldownBuilder::FPulldownContentsLoader::OnPulldownRowRemoved.Broadcast(
		GetTypedOuter<UPulldownContents>(),
		RemovedChangeName
	);
}

void UPulldownListGeneratorBase::NotifyPulldownRowRenamed(const FName& PreChangeName, const FName& PostChangeName)
{
	PulldownBuilder::FPulldownContentsLoader::OnPulldownRowRenamed.Broadcast(
		GetTypedOuter<UPulldownContents>(),
		PreChangeName,
		PostChangeName
	);
}

void UPulldownListGeneratorBase::NotifyPulldownContentsSourceChanged()
{
	PulldownBuilder::FPulldownContentsLoader::OnPulldownContentsSourceChanged.Broadcast(
		GetTypedOuter<UPulldownContents>()
	);
}

bool UPulldownListGeneratorBase::NotifyPulldownRowChanged(const TArray<FName>& PreChangeRowNames, const TArray<FName>& PostChangeRowNames)
{
	bool bModified = true;
	
	if (PreChangeRowNames.Num() < PostChangeRowNames.Num())
	{
		const TArray<FName> AddedRowNames = PostChangeRowNames.FilterByPredicate(
			[&](const FName& RowName) -> bool
			{
				return !PreChangeRowNames.Contains(RowName);
			}
		);
		for (const auto& AddedRowName : AddedRowNames)
		{
			NotifyPulldownRowAdded(AddedRowName);
		}
	}
	else if (PreChangeRowNames.Num() > PostChangeRowNames.Num())
	{
		const TArray<FName> RemovedRowNames = PreChangeRowNames.FilterByPredicate(
			[&](const FName& RowName) -> bool
			{
				return !PostChangeRowNames.Contains(RowName);
			}
		);
		for (const auto& RemovedRowName : RemovedRowNames)
		{
			NotifyPulldownRowRemoved(RemovedRowName);
		}
	}
	else
	{
		bModified = false;
		
		for (int32 Index = 0; Index < PostChangeRowNames.Num(); Index++)
		{
			const FName PreChangeName = PreChangeRowNames[Index];
			const FName PostChangeName = PostChangeRowNames[Index];
			if (PreChangeName != PostChangeName)
			{
				NotifyPulldownRowRenamed(PreChangeName, PostChangeName);
				bModified = true;
			}
		}
	}

	return bModified;
}
