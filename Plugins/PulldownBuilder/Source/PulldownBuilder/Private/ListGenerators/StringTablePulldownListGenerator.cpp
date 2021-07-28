// Copyright 2021 Naotsun. All Rights Reserved.

#include "ListGenerators/StringTablePulldownListGenerator.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"

TArray<TSharedPtr<FPulldownRow>> UStringTablePulldownListGenerator::GetPulldownRows() const
{
	TArray<TSharedPtr<FPulldownRow>> PulldownRows = Super::GetPulldownRows();

	// If the return value of the parent GetDisplayStrings is empty,
	// the list to be displayed in the pull-down menu is generated from
	// the string table in consideration of expansion on the Blueprint side.
	if (PulldownRows.Num() == 0)
	{
		if (UStringTable* StringTable = SourceStringTable.LoadSynchronous())
		{
			StringTable->GetStringTable()->EnumerateSourceStrings(
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
