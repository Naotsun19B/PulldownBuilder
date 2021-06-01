// Copyright 2021 Naotsun. All Rights Reserved.

#include "ListGenerator/StringTablePulldownListGenerator.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"

TArray<TSharedPtr<FString>> UStringTablePulldownListGenerator::GetDisplayStrings() const
{
	TArray<TSharedPtr<FString>> DisplayStrings = Super::GetDisplayStrings();

	// If the return value of the parent GetDisplayStrings is empty,
	// the list to be displayed in the pull-down menu is generated from
	// the string table in consideration of expansion on the Blueprint side.
	if (DisplayStrings.Num() == 0)
	{
		if (UStringTable* StringTable = SourceStringTable.LoadSynchronous())
		{
			StringTable->GetStringTable()->EnumerateSourceStrings(
				[&](const FString& InKey, const FString& InSourceString) -> bool
				{
					if (FName(InKey) != NAME_None)
					{
						DisplayStrings.Add(MakeShared<FString>(InKey));
					}
					return true;
				}
			);
		}
	}

	return DisplayStrings;
}
