// Copyright 2021 Naotsun. All Rights Reserved.

#include "ListGenerator/NameArrayPulldownListGenerator.h"

TArray<TSharedPtr<FString>> UNameArrayPulldownListGenerator::GetDisplayStrings() const
{
	TArray<TSharedPtr<FString>> DisplayStrings = Super::GetDisplayStrings();

	// If the return value of the parent GetDisplayStrings is empty,
	// the list to be displayed in the pull-down menu is generated from
	// the name array in consideration of expansion on the Blueprint side.
	if (DisplayStrings.Num() == 0)
	{
		for (const auto& SourceName : SourceNameArray)
		{
			DisplayStrings.Add(MakeShared<FString>(SourceName.ToString()));
		}
	}

	return DisplayStrings;
}
