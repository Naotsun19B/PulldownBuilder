// Copyright 2021 Naotsun. All Rights Reserved.

#include "PulldownContentsAsset/PulldownContents.h"

TArray<TSharedPtr<FString>> UPulldownContents::GetDisplayStrings()
{
	TArray<TSharedPtr<FString>> DisplayStrings;
	TArray<FString> DisplayStringsFromBlueprint = GetDisplayStringsFromBlueprint();
	for (const auto& DisplayStringFromBlueprint : DisplayStringsFromBlueprint)
	{
		DisplayStrings.Add(MakeShared<FString>(DisplayStringFromBlueprint));
	}

	return DisplayStrings;
}
