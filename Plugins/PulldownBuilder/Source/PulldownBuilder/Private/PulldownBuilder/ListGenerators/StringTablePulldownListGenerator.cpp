// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/StringTablePulldownListGenerator.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(StringTablePulldownListGenerator)
#endif

UStringTablePulldownListGenerator::UStringTablePulldownListGenerator()
	: bInitialized(false)
{
}

void UStringTablePulldownListGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UStringTablePulldownListGenerator, SourceStringTable))
	{
		VerifyDefaultValue();
		NotifyPulldownContentsSourceChanged();
		PreChangeKeys.Empty();
		bInitialized = false;
	}
}

FPulldownRows UStringTablePulldownListGenerator::GetPulldownRows(
	const TArray<UObject*>& OuterObjects,
	const FStructContainer& StructInstance
) const
{
	FPulldownRows PulldownRows;
	if (const UStringTable* StringTable = SourceStringTable.LoadSynchronous())
	{
		const FStringTableConstRef& StringTableInternal = StringTable->GetStringTable();
		StringTableInternal->EnumerateSourceStrings(
			[&](const FString& InKey, const FString& InSourceString) -> bool
			{
				if (FName(*InKey) != NAME_None)
				{
					PulldownRows.Add(FPulldownRow(InKey, FText::FromString(InSourceString)));
				}
					
				return true;
			}
		);
	}
	
	return PulldownRows;
}

bool UStringTablePulldownListGenerator::HasSourceAsset_Implementation() const
{
	return true;
}

FString UStringTablePulldownListGenerator::GetSourceAssetName_Implementation() const
{
	return SourceStringTable.GetAssetName();
}

void UStringTablePulldownListGenerator::Tick(float DeltaTime)
{
	if (!bInitialized)
	{
		CacheStringTableKeys(PreChangeKeys);
		bInitialized = true;
	}
	
	TArray<FName> PostChangeKeys;
	CacheStringTableKeys(PostChangeKeys);

	PreSourceStringTableModify();
	
	if (NotifyPulldownRowChanged(PreChangeKeys, PostChangeKeys))
	{
		PostSourceStringTableModify();
		VerifyDefaultValue();
		
		PreChangeKeys = MoveTemp(PostChangeKeys);
	}
}

bool UStringTablePulldownListGenerator::IsTickable() const
{
	return (
		!IsTemplate() &&
		SourceStringTable.IsValid()
	);
}

TStatId UStringTablePulldownListGenerator::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UStringTablePulldownListGenerator, STATGROUP_Tickables);
}

void UStringTablePulldownListGenerator::CacheStringTableKeys(TArray<FName>& StringTableKeys) const
{
	const auto* StringTable = SourceStringTable.LoadSynchronous();
	if (!IsValid(StringTable))
	{
		return;
	}

	StringTableKeys.Empty();
	
	const FStringTableConstRef& StringTableInternal = StringTable->GetStringTable();
	StringTableInternal->EnumerateSourceStrings(
		[&](const FString& InKey, const FString& InSourceString) -> bool
		{
			if (FName(*InKey) != NAME_None)
			{
				StringTableKeys.Add(*InKey);
			}
			
			return true;
		}
	);
}

TArray<FName> UStringTablePulldownListGenerator::GetDefaultValueOptions_Implementation() const
{
	TArray<FName> StringTableKeys;
	CacheStringTableKeys(StringTableKeys);
	return StringTableKeys;
}
