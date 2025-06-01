// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/NameArrayPulldownListGenerator.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(NameArrayPulldownListGenerator)
#endif

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

		PreSourceNameArrayModify();
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

		PostSourceNameArrayModify();
		
		if (NotifyPulldownRowChanged(PreChangeRowNames, PostChangeRowNames))
		{
			PreChangeRowNames.Empty();
		}

		VerifyDefaultValue();
	}
}

FPulldownRows UNameArrayPulldownListGenerator::GetPulldownRows(
	const TArray<UObject*>& OuterObjects,
	const FStructContainer& StructInstance
) const
{
	FPulldownRows PulldownRows(SourceNameArray);
	for (const auto& SourceName : SourceNameArray)
	{
		if (SourceName.Key != NAME_None)
		{
			PulldownRows.Add(FPulldownRow(SourceName.Key.ToString(), FText::FromName(SourceName.Value)));
		}
	}
	
	return PulldownRows;
}

TArray<FName> UNameArrayPulldownListGenerator::GetDefaultValueOptions_Implementation() const
{
	TArray<FName> DefaultValueOptions;
	SourceNameArray.GenerateKeyArray(DefaultValueOptions);
	return DefaultValueOptions;
}
