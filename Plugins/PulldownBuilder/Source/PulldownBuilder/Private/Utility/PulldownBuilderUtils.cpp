// Copyright 2021 Naotsun. All Rights Reserved.

#include "Utility/PulldownBuilderUtils.h"
#include "PulldownBuilderGlobals.h"
#include "Asset/PulldownContents.h"
#include "PulldownStructBase.h"
#include "NativeLessPulldownStruct.h"
#include "EdGraphSchema_K2.h"

bool FPulldownBuilderUtils::IsChildStruct(const UScriptStruct* InSuperStruct, const UScriptStruct* InTestStruct)
{
	// Check that the "InTestStruct" inherits "InSuperStruct" rather than being temporary.
	bool bValidStruct = false;
	bool bBasedOnSuperStruct = false;
	
	if (IsValid(InSuperStruct) && IsValid(InTestStruct))
	{
		bValidStruct = (InTestStruct->GetOutermost() != GetTransientPackage());
		bBasedOnSuperStruct = (InTestStruct->IsChildOf(InSuperStruct) && (InTestStruct != InSuperStruct));
	}

	return (bValidStruct && bBasedOnSuperStruct);
}

bool FPulldownBuilderUtils::IsPulldownStruct(const UScriptStruct* InTestStruct)
{
	const bool bBasedOnPulldownStructBase = IsChildStruct(FPulldownStructBase::StaticStruct(), InTestStruct);
	const bool bBasedOnNativeLessPulldownStruct = IsNativeLessPulldownStruct(InTestStruct);
	
	// Note: The return value of "UEdGraphSchema_K2::IsAllowableBlueprintVariableType" is true
	// because the structure that inherits any of the structures will be automatically added
	// "BlueprintType" unless "NotBlueprintType" and "BlueprintInternalUseOnly" is added in
	// the USTRUCT of the child structure.
	const bool bIsBlueprintType = UEdGraphSchema_K2::IsAllowableBlueprintVariableType(InTestStruct);

	return (bBasedOnPulldownStructBase && !bBasedOnNativeLessPulldownStruct && bIsBlueprintType);
}

bool FPulldownBuilderUtils::IsNativeLessPulldownStruct(const UScriptStruct* InTestStruct)
{
	const bool bIsNativeLessPulldownStruct = (FNativeLessPulldownStruct::StaticStruct() == InTestStruct);
	const bool bBasedOnNativeLessPulldownStruct = IsChildStruct(FNativeLessPulldownStruct::StaticStruct(), InTestStruct);
	const bool bIsBlueprintType = UEdGraphSchema_K2::IsAllowableBlueprintVariableType(InTestStruct);

	return ((bIsNativeLessPulldownStruct || bBasedOnNativeLessPulldownStruct) && bIsBlueprintType);
}

void FPulldownBuilderUtils::EnumeratePulldownContents(const TFunction<bool(UPulldownContents*)>& Callback)
{
	for (TObjectIterator<UPulldownContents> ObjectItr; ObjectItr; ++ObjectItr)
	{
		UPulldownContents* PulldownContents = *ObjectItr;
		if (IsValid(PulldownContents))
		{
			Callback(PulldownContents);
		}
	}
}

TArray<UPulldownContents*> FPulldownBuilderUtils::GetAllPulldownContents()
{
	TArray<UPulldownContents*> PulldownContentsList;
	EnumeratePulldownContents([&](UPulldownContents* PulldownContents) -> bool
	{
		PulldownContentsList.Add(PulldownContents);
		return true;
	});
	
	return PulldownContentsList;
}

UPulldownContents* FPulldownBuilderUtils::FindPulldownContentsByStruct(const UScriptStruct* InStruct)
{
	UPulldownContents* FoundItem = nullptr;
	EnumeratePulldownContents([&](UPulldownContents* PulldownContents) -> bool
    {
        if (InStruct == PulldownContents->GetPulldownStructType().SelectedStruct)
        {
        	FoundItem = PulldownContents;
            return false;
        }
		
        return true;
    });

	return FoundItem;
}

UPulldownContents* FPulldownBuilderUtils::FindPulldownContentsByName(const FName& InName)
{
	UPulldownContents* FoundItem = nullptr;
	EnumeratePulldownContents([&](UPulldownContents* PulldownContents) -> bool
	{
		if (InName == PulldownContents->GetFName())
		{
			FoundItem = PulldownContents;
			return false;
		}
		
		return true;
	});

	return FoundItem;
}

TArray<TSharedPtr<FString>> FPulldownBuilderUtils::GetDisplayStringsFromStruct(const UScriptStruct* InStruct)
{
	TArray<TSharedPtr<FString>> DisplayStrings;
	if (UPulldownContents* FoundItem = FindPulldownContentsByStruct(InStruct))
	{
		DisplayStrings = FoundItem->GetDisplayStrings();
	}

	return DisplayStrings;
}

TArray<TSharedPtr<FString>> FPulldownBuilderUtils::GetEmptyDisplayStrings()
{
	TArray<TSharedPtr<FString>> EmptySelectableValues;
	EmptySelectableValues.Add(MakeShared<FString>(FName(NAME_None).ToString()));
	return EmptySelectableValues;
}

bool FPulldownBuilderUtils::IsRegisteredPulldownStruct(const UScriptStruct* InStruct)
{
	bool bIsRegistered = false;
	EnumeratePulldownContents([&](UPulldownContents* PulldownContents) -> bool
    {
		if (InStruct == PulldownContents->GetPulldownStructType().SelectedStruct)
		{
			bIsRegistered = true;
			return false;
		}
		
        return true;
    });
    
	return bIsRegistered;
}

FString FPulldownBuilderUtils::GenerateStructDefaultValueString(const UScriptStruct* InStruct)
{
	check(InStruct);
	
	FString DefaultValueString = TEXT("(");	
#if BEFORE_UE_4_24
	for (TFieldIterator<UProperty> PropertyItr(InStruct); PropertyItr; ++PropertyItr)
#else
	for (TFieldIterator<FProperty> PropertyItr(InStruct); PropertyItr; ++PropertyItr)
#endif
	{
#if BEFORE_UE_4_24
		UProperty* Property = *PropertyItr;
#else
		FProperty* Property = *PropertyItr;
#endif
		if (Property == nullptr)
		{
			continue;
		}

		DefaultValueString += FString::Printf(TEXT("%s=,"), *Property->GetName());
	}
	DefaultValueString[DefaultValueString.Len() - 1] = TEXT(')');

	return DefaultValueString;
}
