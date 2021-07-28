// Copyright 2021 Naotsun. All Rights Reserved.

#include "ListGenerators/PulldownListGeneratorBase.h"
#include "Assets/PulldownContents.h"
#include "RowNameUpdaters/RowNameUpdaterBase.h"
#include "Utilities/PulldownBuilderSettings.h"

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
	return FString();
}

void UPulldownListGeneratorBase::UpdateDisplayStrings(const FName& PreChangeName, const FName& PostChangeName)
{
	auto* Settings = GetDefault<UPulldownBuilderSettings>();
	check(Settings);
	if (!Settings->bShouldUpdateWhenSourceRowNameChanged)
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
