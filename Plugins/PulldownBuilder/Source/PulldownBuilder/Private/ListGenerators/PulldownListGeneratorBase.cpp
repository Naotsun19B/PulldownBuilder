// Copyright 2021 Naotsun. All Rights Reserved.

#include "ListGenerators/PulldownListGeneratorBase.h"
#include "Assets/PulldownContents.h"
#include "RowNameUpdaters/RowNameUpdaterBase.h"
#include "Utilities/PulldownBuilderSettings.h"

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
