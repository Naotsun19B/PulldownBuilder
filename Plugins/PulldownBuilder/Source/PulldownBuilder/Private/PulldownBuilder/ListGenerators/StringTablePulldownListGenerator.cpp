﻿// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/StringTablePulldownListGenerator.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"

UStringTablePulldownListGenerator::UStringTablePulldownListGenerator()
	: bInitialized(false)
{
}

TArray<TSharedPtr<FPulldownRow>> UStringTablePulldownListGenerator::GetPulldownRows(
	const TArray<UObject*>& OuterObjects,
	const FStructContainer& StructInstance
) const
{
	TArray<TSharedPtr<FPulldownRow>> PulldownRows = Super::GetPulldownRows(OuterObjects, StructInstance);

	// If the return value of the parent GetDisplayStrings is empty,
	// the list to be displayed in the pull-down menu is generated from
	// the string table in consideration of expansion on the Blueprint side.
	if (PulldownRows.Num() == 0)
	{
		if (const UStringTable* StringTable = SourceStringTable.LoadSynchronous())
		{
			const FStringTableConstRef& StringTableInternal = StringTable->GetStringTable();
			StringTableInternal->EnumerateSourceStrings(
				[&](const FString& InKey, const FString& InSourceString) -> bool
				{
					if (FName(*InKey) != NAME_None)
					{
						PulldownRows.Add(MakeShared<FPulldownRow>(InKey, InSourceString));
					}
					
					return true;
				}
			);
		}
	}

	return PulldownRows;
}

bool UStringTablePulldownListGenerator::HasSourceAsset() const
{
	return true;
}

FString UStringTablePulldownListGenerator::GetSourceAssetName() const
{
	if (const UStringTable* StringTable = SourceStringTable.LoadSynchronous())
	{
		return StringTable->GetName();
	}

	return TEXT("SourceStringTable is not set");
}

void UStringTablePulldownListGenerator::Tick(float DeltaTime)
{
	if (!bInitialized)
	{
		CacheStringTableKeys(PreChangeRowNames);
		bInitialized = true;
	}
	
	TArray<FName> PostChangeRowNames;
	CacheStringTableKeys(PostChangeRowNames);

	if (NotifyPulldownRowChanged(PreChangeRowNames, PostChangeRowNames))
	{
		PreChangeRowNames = MoveTemp(PostChangeRowNames);
	}
}

bool UStringTablePulldownListGenerator::IsTickable() const
{
	return !IsTemplate();
}

TStatId UStringTablePulldownListGenerator::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UStringTablePulldownListGenerator, STATGROUP_Tickables);
}

void UStringTablePulldownListGenerator::CacheStringTableKeys(TArray<FName>& StringTableKeys) const
{
	const auto* StringTable = SourceStringTable.LoadSynchronous();
	if (!IsValid(StringTable))
	{
		return;
	}

	StringTableKeys.Empty();
	
	const FStringTableConstRef& StringTableInternal = StringTable->GetStringTable();
	StringTableInternal->EnumerateSourceStrings(
		[&](const FString& InKey, const FString& InSourceString) -> bool
		{
			if (FName(*InKey) != NAME_None)
			{
				StringTableKeys.Add(*InKey);
			}
			
			return true;
		}
	);
}
