// Copyright 2021 Naotsun. All Rights Reserved.

#include "ListGenerator/PulldownListGeneratorBase.h"
#include "PulldownBuilderGlobals.h"
#include "Asset/PulldownContents.h"
#include "RowNameUpdater/RowNameUpdaterBase.h"

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

void UPulldownListGeneratorBase::UpdateDisplayStrings(const FName& PreChangeName, const FName& PostChangeName)
{
	auto* PulldownContents = Cast<UPulldownContents>(GetOuter());
	if (!IsValid(PulldownContents))
	{
		return;
	}
	
	UE_LOG(LogPulldownBuilder, Log, TEXT("[%s] %s -> %s"), *PulldownContents->GetName(), *PreChangeName.ToString(), *PostChangeName.ToString());

	URowNameUpdaterBase::UpdateRowNames(PulldownContents, PreChangeName, PostChangeName);
}
