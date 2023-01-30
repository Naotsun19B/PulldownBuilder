// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/RowNameUpdaters/RowNameUpdaterBase.h"
#include "PulldownBuilder/Utilities/PulldownBuilderRedirectSettings.h"
#include "PulldownBuilder/Utilities/PulldownBuilderMessageLog.h"
#include "Misc/UObjectToken.h"
#include "UObject/UObjectThreadContext.h"

#define LOCTEXT_NAMESPACE "PulldownListGeneratorBase"

const FName UPulldownListGeneratorBase::FilterPulldownStructTypesName = TEXT("FilterPulldownStructTypes");

TArray<TSharedPtr<FPulldownRow>> UPulldownListGeneratorBase::GetPulldownRows(
	const TArray<UObject*>& OuterObjects,
	const FStructContainer& StructInstance
) const
{
	TArray<TSharedPtr<FPulldownRow>> PulldownRows;
	TArray<FPulldownRow> PulldownRowsFromBlueprint = GetPulldownRowsFromBlueprint(OuterObjects, StructInstance);
	for (const auto& PulldownRowFromBlueprint : PulldownRowsFromBlueprint)
	{
		PulldownRows.Add(MakeShared<FPulldownRow>(PulldownRowFromBlueprint));
	}

	return PulldownRows;
}

bool UPulldownListGeneratorBase::HasSourceAsset() const
{
	// Blueprint functions are not available during routing post load.
	if (!FUObjectThreadContext::Get().IsRoutingPostLoad)
	{
		return HasSourceAssetFromBlueprint();
	}

	return false;
}

FString UPulldownListGeneratorBase::GetSourceAssetName() const
{
	unimplemented(); // Be sure to override it when using this function.
	return GetSourceAssetNameFromBlueprint();
}

TArray<UScriptStruct*> UPulldownListGeneratorBase::GetFilterPulldownStructTypes() const
{
	TArray<UScriptStruct*> FilterPulldownStructTypes;

	TArray<FString> FilterPulldownStructTypeNames;
	if (const UClass* Class = GetClass())
	{
		if (Class->HasMetaData(FilterPulldownStructTypesName))
		{
			const FString MetaString = Class->GetMetaData(FilterPulldownStructTypesName);
			MetaString.ParseIntoArray(FilterPulldownStructTypeNames, TEXT(","));
		}
	}

	const TArray<FName>& FilterPulldownStructTypeNamesFromBlueprint = GetFilterPulldownStructTypesFromBlueprint();
	for (const auto& FilterPulldownStructTypeNameFromBlueprint : FilterPulldownStructTypeNamesFromBlueprint)
	{
		FilterPulldownStructTypeNames.Add(FilterPulldownStructTypeNameFromBlueprint.ToString());
	}

	FilterPulldownStructTypes.Reserve(FilterPulldownStructTypeNames.Num());
	for (auto& FilterPulldownStructTypeName : FilterPulldownStructTypeNames)
	{
		FilterPulldownStructTypeName.TrimStartAndEndInline();

		auto* FoundStruct = FindObject<UScriptStruct>(
#if BEFORE_UE_5_00
			ANY_PACKAGE,
#else
			nullptr,
#endif
			*FilterPulldownStructTypeName
		);
		if (IsValid(FoundStruct))
		{
			FilterPulldownStructTypes.Add(FoundStruct);
		}
	}
	
	return FilterPulldownStructTypes;
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
	
	PulldownBuilder::FPulldownBuilderMessageLog MessageLog;
	MessageLog.Info(
		FText::Format(
			LOCTEXT("NotifyUpdateDisplayStrings", "The source row name for \"{0}\" has changed from \"{1}\" to \"{2}\"."),
			FText::FromString(PulldownContents->GetName()),
			FText::FromName(PreChangeName),
			FText::FromName(PostChangeName)
		)
	)->AddToken(FUObjectToken::Create(PulldownContents));
	
	URowNameUpdaterBase::UpdateRowNames(PulldownContents, PreChangeName, PostChangeName);
}

#undef LOCTEXT_NAMESPACE
