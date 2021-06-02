// Copyright 2021 Naotsun. All Rights Reserved.

#include "RowNameUpdater/DataTableUpdater.h"
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
#if BEFORE_UE_4_24
			for (TFieldIterator<UStructProperty> PropertyItr(RowType); PropertyItr; ++PropertyItr)
			{
				UStructProperty* StructProperty = *PropertyItr;
#else
			for (TFieldIterator<FStructProperty> PropertyItr(RowType); PropertyItr; ++PropertyItr)
			{
				FStructProperty* StructProperty = *PropertyItr;
#endif 
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
