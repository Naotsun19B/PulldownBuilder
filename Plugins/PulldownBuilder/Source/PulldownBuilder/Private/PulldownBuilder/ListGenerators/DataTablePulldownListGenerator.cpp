// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/DataTablePulldownListGenerator.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#if !BEFORE_UE_4_24
#include "UObject/TextProperty.h"
#endif

const FString UDataTablePulldownListGenerator::TooltipPropertyMeta			= TEXT("TooltipProperty");
const FString UDataTablePulldownListGenerator::DefaultPulldownTooltipName	= TEXT("PulldownTooltip");

TArray<TSharedPtr<FPulldownRow>> UDataTablePulldownListGenerator::GetPulldownRows(
	const TArray<UObject*>& OuterObjects,
	const FStructContainer& StructInstance
) const
{
	TArray<TSharedPtr<FPulldownRow>> PulldownRows = Super::GetPulldownRows(OuterObjects, StructInstance);

	// If the return value of the parent GetDisplayStrings is empty,
	// the list to be displayed in the pull-down menu is generated from
	// the data table in consideration of expansion on the Blueprint side.
	if (PulldownRows.Num() == 0)
	{
		if (UDataTable* DataTable = SourceDataTable.LoadSynchronous())
		{
			const UScriptStruct* RowStruct = DataTable->GetRowStruct();
			const TMap<FName, uint8*>& RowMap = DataTable->GetRowMap();
			for (const auto& Pair : RowMap)
			{
				const FName& RowName = Pair.Key;
				uint8* RowData = Pair.Value;
				if (RowName == NAME_None)
				{
					continue;
				}

				FString TooltipString;
				if (FindTooltip(RowStruct, RowData, TooltipString))
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

	PreSourceDataTableModify();
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

	PostSourceDataTableModify();
}

bool UDataTablePulldownListGenerator::FindTooltip(const UScriptStruct* RowStruct, uint8* RowData, FString& TooltipString) const
{
	check(IsValid(RowStruct));

	FString TooltipPropertyName = DefaultPulldownTooltipName;
	if (RowStruct->HasMetaData(*TooltipPropertyMeta))
	{
		TooltipPropertyName = RowStruct->GetMetaData(*TooltipPropertyMeta);
	}

#if BEFORE_UE_4_24
	for (UProperty* Property : TFieldRange<UProperty>(RowStruct))
#else
	for (FProperty* Property : TFieldRange<FProperty>(RowStruct))
#endif
	{
		if (Property == nullptr)
		{
			continue;
		}

		if (Property->GetName() != TooltipPropertyName)
		{
			continue;	
		}

#if BEFORE_UE_4_24
		if (Property->IsA<UStrProperty>())
#else
		if (Property->IsA<FStrProperty>())
#endif
		{
			if (const auto* ValuePtr = Property->ContainerPtrToValuePtr<FString>(RowData))
			{
				TooltipString = *ValuePtr;
				return true;
			}
		}

#if BEFORE_UE_4_24
		if (Property->IsA<UNameProperty>())
#else
		if (Property->IsA<FNameProperty>())
#endif
		{
			if (const auto* ValuePtr = Property->ContainerPtrToValuePtr<FName>(RowData))
			{
				TooltipString = ValuePtr->ToString();
				return true;
			}
		}

#if BEFORE_UE_4_24
		if (Property->IsA<UTextProperty>())
#else
		if (Property->IsA<FTextProperty>())
#endif
		{
			if (const auto* ValuePtr = Property->ContainerPtrToValuePtr<FText>(RowData))
			{
				TooltipString = ValuePtr->ToString();
				return true;
			}
		}
	}

	return false;
}
