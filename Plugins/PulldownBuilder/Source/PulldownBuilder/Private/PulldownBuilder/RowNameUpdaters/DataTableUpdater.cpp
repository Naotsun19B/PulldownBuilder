// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownBuilder/RowNameUpdaters/DataTableUpdater.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "Engine/DataTable.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(DataTableUpdater)
#endif

void UDataTableUpdater::UpdateRowNamesInternal(
	UPulldownContents* PulldownContents,
	const FName& PreChangeSelectedValue,
	const FName& PostChangeSelectedValue
)
{
	EnumerateAssets<UDataTable>([&](UDataTable* DataTable) -> bool
	{
		bool bIsModified = false;

		const UScriptStruct* RowType = DataTable->GetRowStruct();
		if (!IsValid(RowType))
		{
			return false;
		}

		const TMap<FName, uint8*>& RowMap = DataTable->GetRowMap();
		for (const TPair<FName, uint8*>& Pair : RowMap)
		{
			uint8* RowData = Pair.Value;
			if (RowData == nullptr)
			{
				continue;
			}

			if (UpdateMemberVariables(
				RowType,
				RowData,
				PulldownContents,
				PreChangeSelectedValue,
				PostChangeSelectedValue
			))
			{
				bIsModified = true;
			}
		}

		return bIsModified;
	});
}
