// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/DataTablePulldownListGenerator.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

const FString UDataTablePulldownListGenerator::DefaultPulldownTooltipName = TEXT("PulldownTooltip");
const FString UDataTablePulldownListGenerator::TooltipPropertyMeta = TEXT("TooltipProperty");

TArray<TSharedPtr<FPulldownRow>> UDataTablePulldownListGenerator::GetPulldownRows(const TArray<UObject*>& OuterObjects) const
{
	TArray<TSharedPtr<FPulldownRow>> PulldownRows = Super::GetPulldownRows(OuterObjects);

	// If the return value of the parent GetDisplayStrings is empty,
	// the list to be displayed in the pull-down menu is generated from
	// the data table in consideration of expansion on the Blueprint side.
	if (PulldownRows.Num() == 0)
	{
		if (UDataTable* DataTable = SourceDataTable.LoadSynchronous())
		{
			const UScriptStruct* RowStruct = DataTable->GetRowStruct();
			const TMap<FName, uint8*>& RowMap = DataTable->GetRowMap();
			const TArray<FName>& RowNames = DataTable->GetRowNames();
			for (const auto& RowName : RowNames)
			{
				if (RowName == NAME_None)
				{
					continue;
				}

				FString TooltipString;
				if (FindTooltip(RowStruct, RowMap[RowName], TooltipString))
				{
					PulldownRows.Add(MakeShared<FPulldownRow>(RowName, *TooltipString));
				}
				else
				{
					PulldownRows.Add(MakeShared<FPulldownRow>(RowName));
				}
			}
		}
	}

	return PulldownRows;
}

bool UDataTablePulldownListGenerator::HasSourceAsset() const
{
	return true;
}

FString UDataTablePulldownListGenerator::GetSourceAssetName() const
{
	if (const UDataTable* DataTable = SourceDataTable.LoadSynchronous())
	{
		return DataTable->GetName();
	}

	return TEXT("SourceDataTable is not set");
}

void UDataTablePulldownListGenerator::PreChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info)
{
	if (!IsValid(Changed) ||
		Changed != SourceDataTable.LoadSynchronous() ||
		Info != FDataTableEditorUtils::EDataTableChangeInfo::RowList
	)
	{
		return;
	}

	PreChangeRowList = Changed->GetRowNames();
}

void UDataTablePulldownListGenerator::PostChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info)
{
	if (!IsValid(Changed) ||
		Changed != SourceDataTable.LoadSynchronous() ||
		Info != FDataTableEditorUtils::EDataTableChangeInfo::RowList
	)
	{
		return;
	}

	const TArray<FName> PostChangeRowList = Changed->GetRowNames();
	if (PreChangeRowList.Num() == PostChangeRowList.Num())
	{
		for (int32 Index = 0; Index < PostChangeRowList.Num(); Index++)
		{
			const FName PreChangeName = PreChangeRowList[Index];
			const FName PostChangeName = PostChangeRowList[Index];
			if (PreChangeName != PostChangeName)
			{
				UpdateDisplayStrings(PreChangeName, PostChangeName);
			}
		}
	}

	PreChangeRowList.Empty();
}

bool UDataTablePulldownListGenerator::FindTooltip(const UScriptStruct* RowStruct, uint8* RowData, FString& TooltipString) const
{
	check(IsValid(RowStruct));

	FString TooltipPropertyName = DefaultPulldownTooltipName;
	if (const FString* Meta = RowStruct->FindMetaData(*TooltipPropertyMeta))
	{
		TooltipPropertyName = *Meta;
	}
	
#if BEFORE_UE_4_24
	for (UStrProperty* StringProperty : TFieldRange<UStrProperty>(RowStruct))
#else
	for (FStrProperty* StringProperty : TFieldRange<FStrProperty>(RowStruct))
#endif
	{
		if (StringProperty == nullptr)
		{
			continue;
		}

		if (StringProperty->GetName() != TooltipPropertyName)
		{
			continue;	
		}

		if (FString* ValuePtr = StringProperty->ContainerPtrToValuePtr<FString>(RowData))
		{
			TooltipString = *ValuePtr;
			return true;
		}
	}

	return false;
}
