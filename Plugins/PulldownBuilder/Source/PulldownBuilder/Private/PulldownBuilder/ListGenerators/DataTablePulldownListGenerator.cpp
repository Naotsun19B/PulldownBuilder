﻿// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/DataTablePulldownListGenerator.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "UObject/TextProperty.h"

const FString UDataTablePulldownListGenerator::TooltipPropertyMeta			= TEXT("TooltipProperty");
const FString UDataTablePulldownListGenerator::DefaultPulldownTooltipName	= TEXT("PulldownTooltip");

void UDataTablePulldownListGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UDataTablePulldownListGenerator, SourceDataTable))
	{
		NotifyPulldownContentsSourceChanged();
	}
}

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

	PreChangeRowNames = Changed->GetRowNames();

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

	const TArray<FName> PostChangeRowNames = Changed->GetRowNames();
	if (NotifyPulldownRowChanged(PreChangeRowNames, PostChangeRowNames))
	{
		PreChangeRowNames.Empty();
	}
	
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

#if UE_4_25_OR_LATER
	for (FProperty* Property : TFieldRange<FProperty>(RowStruct))
#else
	for (UProperty* Property : TFieldRange<UProperty>(RowStruct))
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

#if UE_4_25_OR_LATER
		if (Property->IsA<FStrProperty>())
#else
		if (Property->IsA<UStrProperty>())
#endif
		{
			if (const auto* ValuePtr = Property->ContainerPtrToValuePtr<FString>(RowData))
			{
				TooltipString = *ValuePtr;
				return true;
			}
		}

#if UE_4_25_OR_LATER
		if (Property->IsA<FNameProperty>())
#else
		if (Property->IsA<UNameProperty>())
#endif
		{
			if (const auto* ValuePtr = Property->ContainerPtrToValuePtr<FName>(RowData))
			{
				TooltipString = ValuePtr->ToString();
				return true;
			}
		}

#if UE_4_25_OR_LATER
		if (Property->IsA<FTextProperty>())
#else
		if (Property->IsA<UTextProperty>())
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
