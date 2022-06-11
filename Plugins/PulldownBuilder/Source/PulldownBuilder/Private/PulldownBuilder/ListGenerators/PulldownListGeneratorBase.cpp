// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/RowNameUpdaters/RowNameUpdaterBase.h"
#include "PulldownBuilder/Utilities/PulldownBuilderRedirectSettings.h"

TArray<TSharedPtr<FPulldownRow>> UPulldownListGeneratorBase::GetPulldownRows() const
{
	TArray<TSharedPtr<FPulldownRow>> PulldownRows;
	TArray<FPulldownRow> PulldownRowsFromBlueprint = GetPulldownRowsFromBlueprint();
	for (const auto& PulldownRowFromBlueprint : PulldownRowsFromBlueprint)
	{
		PulldownRows.Add(MakeShared<FPulldownRow>(PulldownRowFromBlueprint));
	}

	return PulldownRows;
}

bool UPulldownListGeneratorBase::HasSourceAsset() const
{
	return false;
}

FString UPulldownListGeneratorBase::GetSourceAssetName() const
{
	unimplemented(); // Be sure to override it when using this function.
	return {};
}

void UPulldownListGeneratorBase::UpdateDisplayStrings(const FName& PreChangeName, const FName& PostChangeName)
{
	const auto& Settings = UPulldownBuilderRedirectSettings::Get();
	if (!Settings.bShouldUpdateWhenSourceRowNameChanged)
	{
		return;
	}
	
	auto* PulldownContents = Cast<UPulldownContents>(GetOuter());
	if (!IsValid(PulldownContents))
	{
		return;
	}

	UE_LOG(LogPulldownBuilder, Log, TEXT(" The source row name for \"%s\" has changed from \"%s\" to \"%s\"."), *PulldownContents->GetName(), *PreChangeName.ToString(), *PostChangeName.ToString());

	URowNameUpdaterBase::UpdateRowNames(PulldownContents, PreChangeName, PostChangeName);
}
