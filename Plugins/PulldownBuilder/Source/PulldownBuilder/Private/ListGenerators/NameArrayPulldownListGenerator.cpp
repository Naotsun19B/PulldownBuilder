// Copyright 2021 Naotsun. All Rights Reserved.

#include "ListGenerators/NameArrayPulldownListGenerator.h"

#if BEFORE_UE_4_24
void UNameArrayPulldownListGenerator::PreEditChange(UProperty* PropertyAboutToChange)
#else
void UNameArrayPulldownListGenerator::PreEditChange(FProperty* PropertyAboutToChange)
#endif
{
	Super::PreEditChange(PropertyAboutToChange);

	if (PropertyAboutToChange == nullptr)
	{
		return;
	}

	if (PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(UNameArrayPulldownListGenerator, SourceNameArray))
	{
		PreChangeNameArray = SourceNameArray;
	}
}

void UNameArrayPulldownListGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UNameArrayPulldownListGenerator, SourceNameArray))
	{
		// Filter so that there are no multiple elements with the same value.
		TArray<FName> FilteredArray;
		for (const auto& SourceName : SourceNameArray)
		{
			if (!FilteredArray.Contains(SourceName))
			{
				FilteredArray.Add(SourceName);
			}
		}
		SourceNameArray = FilteredArray;

		if (PreChangeNameArray.Num() == SourceNameArray.Num())
		{
			for (int32 Index = 0; Index < SourceNameArray.Num(); Index++)
			{
				const FName PreChangeName = PreChangeNameArray[Index];
				const FName PostChangeName = SourceNameArray[Index];
				if (PreChangeName != PostChangeName &&
					PreChangeName != NAME_None &&
					PostChangeName != NAME_None)
				{
					UpdateDisplayStrings(PreChangeName, PostChangeName);
				}
			}
		}

		PreChangeNameArray.Empty();
	}
}

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
			if (SourceName != NAME_None)
			{
				DisplayStrings.Add(MakeShared<FString>(SourceName.ToString()));
			}
		}
	}

	return DisplayStrings;
}
