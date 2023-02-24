﻿// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownBuilder/RowNameUpdaters/DataTableUpdater.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "Engine/DataTable.h"

void UDataTableUpdater::UpdateRowNamesInternal(
	UPulldownContents* PulldownContents,
	const FName& PreChangeName,
	const FName& PostChangeName
)
{
	EnumerateAssets<UDataTable>([&](UDataTable* DataTable) -> bool
	{
		bool bIsModified = false;
		
		const UScriptStruct* RowType = DataTable->GetRowStruct();
		const TArray<FName> RowNames = DataTable->GetRowNames();
		for (const auto& RowName : RowNames)
		{
#if UE_4_25_OR_LATER
			for (FStructProperty* StructProperty : TFieldRange<FStructProperty>(RowType))
#else
			for (UStructProperty* StructProperty : TFieldRange<UStructProperty>(RowType))
#endif
			{
				if (StructProperty == nullptr)
				{
					continue;
				}

				const TMap<FName, uint8*>& RowMap = DataTable->GetRowMap();
				if (auto* Row = RowMap.Find(RowName))
				{
					if (UpdateMemberVariables(
						RowType,
						*Row,
						PulldownContents,
						PreChangeName,
						PostChangeName
					))
					{
						bIsModified = true;
					}
				}
			}
		}
		
		return bIsModified;
	});
}
