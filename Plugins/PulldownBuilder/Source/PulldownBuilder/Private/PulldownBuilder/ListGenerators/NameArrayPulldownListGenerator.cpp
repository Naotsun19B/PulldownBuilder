// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/NameArrayPulldownListGenerator.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

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
		PreChangeNameArray.Reset();
		SourceNameArray.GenerateKeyArray(PreChangeNameArray);
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
		if (PreChangeNameArray.Num() == SourceNameArray.Num())
		{
			TArray<FName> PostChangeNameArray;
			SourceNameArray.GenerateKeyArray(PostChangeNameArray);
			
			for (int32 Index = 0; Index < SourceNameArray.Num(); Index++)
			{
				const FName PreChangeName = PreChangeNameArray[Index];
				const FName PostChangeName = PostChangeNameArray[Index];
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

TArray<TSharedPtr<FPulldownRow>> UNameArrayPulldownListGenerator::GetPulldownRows(
	const TArray<UObject*>& OuterObjects,
	const FStructContainer& StructInstance
) const
{
	TArray<TSharedPtr<FPulldownRow>> PulldownRows = Super::GetPulldownRows(OuterObjects, StructInstance);

	// If the return value of the parent GetDisplayStrings is empty,
	// the list to be displayed in the pull-down menu is generated from
	// the name array in consideration of expansion on the Blueprint side.
	if (PulldownRows.Num() == 0)
	{
		for (const auto& SourceName : SourceNameArray)
		{
			if (SourceName.Key != NAME_None)
			{
				PulldownRows.Add(MakeShared<FPulldownRow>(SourceName.Key, SourceName.Value));
			}
		}
	}

	return PulldownRows;
}
