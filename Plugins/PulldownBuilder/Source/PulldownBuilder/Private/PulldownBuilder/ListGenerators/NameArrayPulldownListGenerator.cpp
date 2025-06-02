// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/NameArrayPulldownListGenerator.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(NameArrayPulldownListGenerator)
#endif

#if UE_4_25_OR_LATER
void UDEPRECATED_NameArrayPulldownListGenerator::PreEditChange(FProperty* PropertyAboutToChange)
#else
void UDEPRECATED_NameArrayPulldownListGenerator::PreEditChange(UProperty* PropertyAboutToChange)
#endif
{
	Super::PreEditChange(PropertyAboutToChange);

	if (PropertyAboutToChange == nullptr)
	{
		return;
	}

	if (PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(UDEPRECATED_NameArrayPulldownListGenerator, SourceNameArray))
	{
		PreChangeSelectedValues.Reset();
		SourceNameArray.GenerateKeyArray(PreChangeSelectedValues);

		PreSourceNameArrayModify();
	}
}

void UDEPRECATED_NameArrayPulldownListGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UDEPRECATED_NameArrayPulldownListGenerator, SourceNameArray))
	{
		TArray<FName> PostChangeSelectedValues;
		SourceNameArray.GenerateKeyArray(PostChangeSelectedValues);

		PostSourceNameArrayModify();
		
		if (NotifyPulldownRowChanged(PreChangeSelectedValues, PostChangeSelectedValues))
		{
			PreChangeSelectedValues.Empty();
		}

		VerifyDefaultValue();
	}
}

FPulldownRows UDEPRECATED_NameArrayPulldownListGenerator::GetPulldownRows(
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

TArray<FName> UDEPRECATED_NameArrayPulldownListGenerator::GetDefaultValueOptions_Implementation() const
{
	TArray<FName> DefaultValueOptions;
	SourceNameArray.GenerateKeyArray(DefaultValueOptions);
	return DefaultValueOptions;
}
