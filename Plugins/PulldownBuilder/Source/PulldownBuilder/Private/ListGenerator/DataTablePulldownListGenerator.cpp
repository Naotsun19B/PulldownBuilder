// Copyright 2021 Naotsun. All Rights Reserved.

#include "ListGenerator/DataTablePulldownListGenerator.h"

TArray<TSharedPtr<FString>> UDataTablePulldownListGenerator::GetDisplayStrings() const
{
	TArray<TSharedPtr<FString>> DisplayStrings = Super::GetDisplayStrings();

	// If the return value of the parent GetDisplayStrings is empty,
	// the list to be displayed in the pull-down menu is generated from
	// the data table in consideration of expansion on the Blueprint side.
	if (DisplayStrings.Num() == 0)
	{
		if (UDataTable* DataTable = SourceDataTable.LoadSynchronous())
		{
			const TArray<FName>& RowNames = DataTable->GetRowNames();
			for (const auto& RowName : RowNames)
			{
				if (RowName != NAME_None)
				{
					DisplayStrings.Add(MakeShared<FString>(RowName.ToString()));
				}
			}
		}
	}

	return DisplayStrings;
}
