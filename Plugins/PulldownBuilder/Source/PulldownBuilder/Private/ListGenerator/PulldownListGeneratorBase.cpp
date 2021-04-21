// Copyright 2021 Naotsun. All Rights Reserved.

#include "ListGenerator/PulldownListGeneratorBase.h"

TArray<TSharedPtr<FString>> UPulldownListGeneratorBase::GetDisplayStrings() const
{
	TArray<TSharedPtr<FString>> DisplayStrings;
	TArray<FString> DisplayStringsFromBlueprint = GetDisplayStringsFromBlueprint();
	for (const auto& DisplayStringFromBlueprint : DisplayStringsFromBlueprint)
	{
		DisplayStrings.Add(MakeShared<FString>(DisplayStringFromBlueprint));
	}

	return DisplayStrings;
}
