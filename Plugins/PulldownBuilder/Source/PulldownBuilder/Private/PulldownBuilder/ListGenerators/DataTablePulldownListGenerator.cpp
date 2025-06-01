// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/DataTablePulldownListGenerator.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "UObject/TextProperty.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(DataTablePulldownListGenerator)
#endif

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
		VerifyDefaultValue();
		NotifyPulldownContentsSourceChanged();
	}
}

FPulldownRows UDataTablePulldownListGenerator::GetPulldownRows(
	const TArray<UObject*>& OuterObjects,
	const FStructContainer& StructInstance
) const
{
	FPulldownRows PulldownRows;
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

			FPulldownRow NewPulldownRow(RowName.ToString());
				
			FString TooltipString;
			if (FindTooltip(RowStruct, RowData, TooltipString))
			{
				NewPulldownRow.TooltipText = FText::FromString(TooltipString);
			}
				
			PulldownRows.Add(NewPulldownRow);
		}
	}
	
	return PulldownRows;
}

bool UDataTablePulldownListGenerator::HasSourceAsset_Implementation() const
{
	return true;
}

FString UDataTablePulldownListGenerator::GetSourceAssetName_Implementation() const
{
	return SourceDataTable.GetAssetName();
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

	VerifyDefaultValue();
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

TArray<FName> UDataTablePulldownListGenerator::GetDefaultValueOptions_Implementation() const
{
	if (!SourceDataTable.IsValid())
	{
		return {};
	}

	const TMap<FName, uint8*>& RowMap = SourceDataTable->GetRowMap();
	
	TArray<FName> RowNames;
	RowMap.GenerateKeyArray(RowNames);

	return RowNames;
}
