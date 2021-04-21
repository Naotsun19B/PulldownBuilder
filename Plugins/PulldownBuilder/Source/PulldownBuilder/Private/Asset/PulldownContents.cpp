// Copyright 2021 Naotsun. All Rights Reserved.

#include "Asset/PulldownContents.h"
#include "ListGenerator/PulldownListGeneratorBase.h"

TArray<TSharedPtr<FString>> UPulldownContents::GetDisplayStrings() const
{
	TArray<TSharedPtr<FString>> DisplayStrings;
	
	if (IsValid(PulldownListGenerator))
	{
		DisplayStrings = PulldownListGenerator->GetDisplayStrings();
	}

	return DisplayStrings;
}
