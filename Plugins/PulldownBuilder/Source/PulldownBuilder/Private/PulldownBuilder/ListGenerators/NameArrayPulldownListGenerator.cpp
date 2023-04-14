﻿// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/NameArrayPulldownListGenerator.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

#if UE_4_25_OR_LATER
void UNameArrayPulldownListGenerator::PreEditChange(FProperty* PropertyAboutToChange)
#else
void UNameArrayPulldownListGenerator::PreEditChange(UProperty* PropertyAboutToChange)
#endif
{
	Super::PreEditChange(PropertyAboutToChange);

	if (PropertyAboutToChange == nullptr)
	{
		return;
	}

	if (PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(UNameArrayPulldownListGenerator, SourceNameArray))
	{
		PreChangeRowNames.Reset();
		SourceNameArray.GenerateKeyArray(PreChangeRowNames);
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
		TArray<FName> PostChangeRowNames;
		SourceNameArray.GenerateKeyArray(PostChangeRowNames);
		
		if (NotifyPulldownRowChanged(PreChangeRowNames, PostChangeRowNames))
		{
			PreChangeRowNames.Empty();
		}
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
