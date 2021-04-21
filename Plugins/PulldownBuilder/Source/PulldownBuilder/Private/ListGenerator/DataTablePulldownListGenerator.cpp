// Copyright 2021 Naotsun. All Rights Reserved.

#include "ListGenerator/DataTablePulldownListGenerator.h"

TArray<TSharedPtr<FString>> UDataTablePulldownListGenerator::GetDisplayStrings() const
{
	TArray<TSharedPtr<FString>> DisplayStrings = Super::GetDisplayStrings();

	// If the return value of the parent Get Display Strings is empty,
	// the list to be displayed in the pull-down menu is generated from
	// the data table in consideration of expansion on the Blueprint side.
	if (SourceDataTable.IsValid() && DisplayStrings.Num() == 0)
	{
		SourceDataTable->ForeachRow<FTableRowBase>(
			GET_FUNCTION_NAME_STRING_CHECKED(UDataTablePulldownListGenerator, GetDisplayStrings),
			[&](const FName& Key, const FTableRowBase& Value)
		{
			DisplayStrings.Add(MakeShared<FString>(Key.ToString()));
		});
	}

	return DisplayStrings;
}
