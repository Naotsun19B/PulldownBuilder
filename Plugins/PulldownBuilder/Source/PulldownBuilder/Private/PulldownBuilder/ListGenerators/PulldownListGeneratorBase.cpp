// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Assets/PulldownContentsDelegates.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "UObject/UObjectThreadContext.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(PulldownListGeneratorBase)
#endif

const FName UPulldownListGeneratorBase::FilterPulldownStructTypesName = TEXT("FilterPulldownStructTypes");

UPulldownListGeneratorBase::UPulldownListGeneratorBase()
	: bEnableDefaultValue(false)
{
}

void UPulldownListGeneratorBase::PostInitProperties()
{
	UObject::PostInitProperties();

	VerifyDefaultValue();
}

#if UE_4_25_OR_LATER
bool UPulldownListGeneratorBase::CanEditChange(const FProperty* InProperty) const
#else
bool UPulldownListGeneratorBase::CanEditChange(const UProperty* InProperty) const
#endif
{
	bool bCanEditChange = true;
#if UE_4_25_OR_LATER
	if (InProperty != nullptr)
#else
	if (IsValid(InProperty))
#endif
	{
		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UPulldownListGeneratorBase, bEnableDefaultValue))
		{
			const TArray<FName>& DefaultValueOptions = GetDefaultValueOptions();
			bCanEditChange = (!IsEnableCustomDefaultValue() && (DefaultValueOptions.Num() > 0));
		}
		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UPulldownListGeneratorBase, DefaultValue))
		{
			bCanEditChange = (!IsEnableCustomDefaultValue() && bEnableDefaultValue);
		}
	}
	
	return (UObject::CanEditChange(InProperty) && bCanEditChange);
}

FPulldownRows UPulldownListGeneratorBase::InvokeGetPulldownRows(const TArray<UObject*>& OuterObjects, const FStructContainer& StructInstance) const
{
	// Blueprint functions are not available during routing post load.
	TArray<FPulldownRow> PulldownRowsFromBlueprint;
	if (!FUObjectThreadContext::Get().IsRoutingPostLoad)
	{
		PulldownRowsFromBlueprint = GetPulldownRowsFromBlueprint(OuterObjects, StructInstance);
	}
	
	FPulldownRows PulldownRows(PulldownRowsFromBlueprint);
	if (PulldownRows.IsEmpty())
	{
		PulldownRows = GetPulldownRows(OuterObjects, StructInstance);
	}
	
	ApplyDefaultValue(PulldownRows);
	return PulldownRows;
}

FPulldownRows UPulldownListGeneratorBase::GetPulldownRows(const TArray<UObject*>& OuterObjects, const FStructContainer& StructInstance) const
{
	return FPulldownRows::Empty;
}

bool UPulldownListGeneratorBase::HasSourceAsset_Implementation() const
{
	return false;
}

FString UPulldownListGeneratorBase::GetSourceAssetName_Implementation() const
{
	unimplemented(); // Be sure to override it when using this function.
	return {};
}

TArray<UScriptStruct*> UPulldownListGeneratorBase::InvokeGetFilterPulldownStructTypes() const
{
	TArray<UScriptStruct*> FilterPulldownStructTypes = GetFilterPulldownStructTypes();

	// Blueprint functions are not available during routing post load.
	if (!FUObjectThreadContext::Get().IsRoutingPostLoad)
	{
		const TArray<FName>& FilterPulldownStructTypeNamesFromBlueprint = GetFilterPulldownStructTypesFromBlueprint();
		for (const auto& FilterPulldownStructTypeNameFromBlueprint : FilterPulldownStructTypeNamesFromBlueprint)
		{
			FString FilterPulldownStructTypeName = FilterPulldownStructTypeNameFromBlueprint.ToString();
			FilterPulldownStructTypeName.TrimStartAndEndInline();

			auto* FoundStruct = FindObject<UScriptStruct>(
#if UE_5_01_OR_LATER
				nullptr,
#else
				ANY_PACKAGE,
#endif
				*FilterPulldownStructTypeName
			);
			if (IsValid(FoundStruct))
			{
				FilterPulldownStructTypes.Add(FoundStruct);
			}
		}
	}
	
	return FilterPulldownStructTypes;
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
	
	FilterPulldownStructTypes.Reserve(FilterPulldownStructTypeNames.Num());
	for (auto& FilterPulldownStructTypeName : FilterPulldownStructTypeNames)
	{
		FilterPulldownStructTypeName.TrimStartAndEndInline();

		auto* FoundStruct = FindObject<UScriptStruct>(
#if UE_5_01_OR_LATER
			nullptr,
#else
			ANY_PACKAGE,
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

bool UPulldownListGeneratorBase::SupportsSwitchNode_Implementation() const
{
	return true;
}

void UPulldownListGeneratorBase::NotifyPulldownRowAdded(const FName& AddedSelectedValue)
{
	PulldownBuilder::FPulldownContentsDelegates::OnPulldownRowAdded.Broadcast(
		GetTypedOuter<UPulldownContents>(),
		AddedSelectedValue
	);

	UE_LOG(LogPulldownBuilder, Log, TEXT("Detected the addition of the row name that is the source of %s. (+ %s)"), *GetNameSafe(GetTypedOuter<UPulldownContents>()), *AddedSelectedValue.ToString());
}

void UPulldownListGeneratorBase::NotifyPulldownRowRemoved(const FName& RemovedSelectedValue)
{
	PulldownBuilder::FPulldownContentsDelegates::OnPulldownRowRemoved.Broadcast(
		GetTypedOuter<UPulldownContents>(),
		RemovedSelectedValue
	);

	UE_LOG(LogPulldownBuilder, Log, TEXT("Detected removal of the row name that is the source of %s. (- %s)"), *GetNameSafe(GetTypedOuter<UPulldownContents>()), *RemovedSelectedValue.ToString());
}

void UPulldownListGeneratorBase::NotifyPulldownRowRenamed(const FName& PreChangeSelectedValue, const FName& PostChangeSelectedValue)
{
	PulldownBuilder::FPulldownContentsDelegates::OnPulldownRowRenamed.Broadcast(
		GetTypedOuter<UPulldownContents>(),
		PreChangeSelectedValue,
		PostChangeSelectedValue
	);

	UE_LOG(LogPulldownBuilder, Log, TEXT("Detected renaming of row name that is the source of %s. (%s -> %s)"), *GetNameSafe(GetTypedOuter<UPulldownContents>()), *PreChangeSelectedValue.ToString(), *PostChangeSelectedValue.ToString());
}

void UPulldownListGeneratorBase::NotifyPulldownContentsSourceChanged()
{
	PulldownBuilder::FPulldownContentsDelegates::OnPulldownContentsSourceChanged.Broadcast(
		GetTypedOuter<UPulldownContents>()
	);

	UE_LOG(LogPulldownBuilder, Log, TEXT("Detected modification of underlying data of %s."), *GetNameSafe(GetTypedOuter<UPulldownContents>()));
}

bool UPulldownListGeneratorBase::NotifyPulldownRowChanged(const TArray<FName>& PreChangeSelectedValues, const TArray<FName>& PostChangeSelectedValues)
{
	bool bModified = true;
	
	if (PreChangeSelectedValues.Num() < PostChangeSelectedValues.Num())
	{
		const TArray<FName> AddedSelectedValues = PostChangeSelectedValues.FilterByPredicate(
			[&](const FName& SelectedValue) -> bool
			{
				return !PreChangeSelectedValues.Contains(SelectedValue);
			}
		);
		for (const auto& AddedSelectedValue : AddedSelectedValues)
		{
			NotifyPulldownRowAdded(AddedSelectedValue);
		}
	}
	else if (PreChangeSelectedValues.Num() > PostChangeSelectedValues.Num())
	{
		const TArray<FName> RemovedSelectedValues = PreChangeSelectedValues.FilterByPredicate(
			[&](const FName& SelectedValue) -> bool
			{
				return !PostChangeSelectedValues.Contains(SelectedValue);
			}
		);
		for (const auto& RemovedSelectedValue : RemovedSelectedValues)
		{
			NotifyPulldownRowRemoved(RemovedSelectedValue);
		}
	}
	else
	{
		bModified = false;
		
		for (int32 Index = 0; Index < PostChangeSelectedValues.Num(); Index++)
		{
			const FName PreChangeSelectedValue = PreChangeSelectedValues[Index];
			const FName PostChangeSelectedValue = PostChangeSelectedValues[Index];
			if (PreChangeSelectedValue != PostChangeSelectedValue)
			{
				NotifyPulldownRowRenamed(PreChangeSelectedValue, PostChangeSelectedValue);
				bModified = true;
			}
		}
	}

	return bModified;
}

bool UPulldownListGeneratorBase::IsEnableCustomDefaultValue_Implementation() const
{
	return false;
}

TArray<FName> UPulldownListGeneratorBase::GetDefaultValueOptions_Implementation() const
{
	if (!IsEnableCustomDefaultValue() && bEnableDefaultValue)
	{
		unimplemented();
	}
	
	return {};
}

void UPulldownListGeneratorBase::VerifyDefaultValue()
{
	// Blueprint functions are not available during routing post load.
	if (FUObjectThreadContext::Get().IsRoutingPostLoad)
	{
		return;
	}
	
	const TArray<FName> DefaultValueOptions = GetDefaultValueOptions();
	if (DefaultValueOptions.Contains(DefaultValue))
	{
		return;
	}

	bEnableDefaultValue = false;
	DefaultValue = FName();
}

void UPulldownListGeneratorBase::ApplyDefaultValue(FPulldownRows& PulldownRows) const
{
	// Blueprint functions are not available during routing post load.
	if (FUObjectThreadContext::Get().IsRoutingPostLoad)
	{
		return;
	}
	
	if (IsEnableCustomDefaultValue() || !bEnableDefaultValue)
	{
		return;
	}

	PulldownRows.SetDefaultRow(
		[&](const TSharedRef<FPulldownRow>& Row) -> bool
		{
			return (Row->SelectedValue.Equals(DefaultValue.ToString()));
		}
	);
}

void UPulldownListGeneratorBase::ApplyDefaultValueForBlueprint(TArray<FPulldownRow>& PulldownRows)
{
	if (IsEnableCustomDefaultValue() || !bEnableDefaultValue)
	{
		return;
	}
	
	for (auto& PulldownRow : PulldownRows)
	{
		if (PulldownRow.SelectedValue.Equals(DefaultValue.ToString()))
		{
			PulldownRow.bIsDefaultValue = true;
			break;
		}
	}
}
