// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/SimplePulldownListGenerator.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(SimplePulldownListGenerator)
#endif

#if UE_4_25_OR_LATER
void USimplePulldownListGenerator::PreEditChange(FProperty* PropertyAboutToChange)
#else
void USimplePulldownListGenerator::PreEditChange(UProperty* PropertyAboutToChange)
#endif
{
	Super::PreEditChange(PropertyAboutToChange);

	if (PropertyAboutToChange == nullptr)
	{
		return;
	}

	if (PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(USimplePulldownListGenerator, PulldownRows))
	{
		PreChangeSelectedValues.Reset();
		PreChangeSelectedValues.Reserve(PulldownRows.Num());
		for (const auto& PulldownRow : PulldownRows)
		{
			PreChangeSelectedValues.Add(*PulldownRow.SelectedValue);
		}

		PreSourceNameArrayModify();
	}
}

void USimplePulldownListGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(USimplePulldownListGenerator, PulldownRows))
	{
		TArray<FName> PostChangeSelectedValues;
		PostChangeSelectedValues.Reserve(PulldownRows.Num());
		for (const auto& PulldownRow : PulldownRows)
		{
			PostChangeSelectedValues.Add(*PulldownRow.SelectedValue);
		}

		PostSourceNameArrayModify();
		
		if (NotifyPulldownRowChanged(PreChangeSelectedValues, PostChangeSelectedValues))
		{
			PreChangeSelectedValues.Empty();
		}

		VerifyDefaultValue();
	}
}

FPulldownRows USimplePulldownListGenerator::GetPulldownRows(
	const TArray<UObject*>& OuterObjects,
	const FStructContainer& StructInstance
) const
{
	return FPulldownRows(PulldownRows);
}

TArray<FName> USimplePulldownListGenerator::GetDefaultValueOptions_Implementation() const
{
	TArray<FName> DefaultValueOptions;
	DefaultValueOptions.Reserve(PulldownRows.Num());
	for (const auto& PulldownRow : PulldownRows)
	{
		DefaultValueOptions.Add(*PulldownRow.SelectedValue);
	}
	
	return DefaultValueOptions;
}
