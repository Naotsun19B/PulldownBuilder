// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/DataTablePulldownListGenerator.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "UObject/UObjectThreadContext.h"
#include "UObject/TextProperty.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(DataTablePulldownListGenerator)
#endif

const FString UDataTablePulldownListGenerator::TooltipPropertyMeta			= TEXT("TooltipProperty");
const FString UDataTablePulldownListGenerator::DefaultPulldownTooltipName	= TEXT("PulldownTooltip");
const FString UDataTablePulldownListGenerator::TextColorPropertyMeta		= TEXT("TextColorProperty");
const FString UDataTablePulldownListGenerator::DefaultPulldownTextColorName	= TEXT("PulldownTextColor");

void UDataTablePulldownListGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UDataTablePulldownListGenerator, SourceDataTable))
	{
		VerifyDefaultValue();
		NotifyPulldownContentsSourceChanged();
	}
}

FPulldownRows UDataTablePulldownListGenerator::GetPulldownRows(
	const TArray<UObject*>& OuterObjects,
	const FStructContainer& StructInstance
) const
{
	FPulldownRows PulldownRows;
	UDataTable* DataTable = SourceDataTable.LoadSynchronous();
	if (!IsValid(DataTable))
	{
		return PulldownRows;
	}

	const UScriptStruct* RowStruct = DataTable->GetRowStruct();
	if (!IsValid(RowStruct))
	{
		return PulldownRows;
	}

	// The tooltip / text-color property names and the matching FProperty are constant across all rows.
	// Calling FindTooltip / FindTextColor per row would iterate TFieldRange every time; instead, resolve once
	// outside the row loop and dispatch to the cheap extraction helpers per row.
	//
	// NOTE: FindTooltip / FindTextColor remain virtual for back-compat -- subclasses that override them and
	// also override GetPulldownRows can still call them directly. The default GetPulldownRows below uses the
	// inlined fast path.

	FString TooltipPropertyName = DefaultPulldownTooltipName;
	if (RowStruct->HasMetaData(*TooltipPropertyMeta))
	{
		TooltipPropertyName = RowStruct->GetMetaData(*TooltipPropertyMeta);
	}

	FString TextColorPropertyName = DefaultPulldownTextColorName;
	if (RowStruct->HasMetaData(*TextColorPropertyMeta))
	{
		TextColorPropertyName = RowStruct->GetMetaData(*TextColorPropertyMeta);
	}

#if UE_4_25_OR_LATER
	const FProperty* TooltipProperty = nullptr;
	for (const FProperty* Property : TFieldRange<FProperty>(RowStruct))
#else
	const UProperty* TooltipProperty = nullptr;
	for (const UProperty* Property : TFieldRange<UProperty>(RowStruct))
#endif
	{
		if (Property != nullptr && Property->GetName() == TooltipPropertyName)
		{
			TooltipProperty = Property;
			break;
		}
	}

#if UE_4_25_OR_LATER
	const FStructProperty* TextColorProperty = nullptr;
	for (const FStructProperty* Property : TFieldRange<FStructProperty>(RowStruct))
#else
	const UStructProperty* TextColorProperty = nullptr;
	for (const UStructProperty* Property : TFieldRange<UStructProperty>(RowStruct))
#endif
	{
		if (Property != nullptr && Property->GetName() == TextColorPropertyName)
		{
			TextColorProperty = Property;
			break;
		}
	}

	const TMap<FName, uint8*>& RowMap = DataTable->GetRowMap();
	for (const auto& Pair : RowMap)
	{
		const FName& RowName = Pair.Key;
		uint8* RowData = Pair.Value;
		if (RowName == NAME_None || RowData == nullptr)
		{
			continue;
		}

		FPulldownRow NewPulldownRow(RowName.ToString());

		if (TooltipProperty != nullptr)
		{
#if UE_4_25_OR_LATER
			if (TooltipProperty->IsA<FStrProperty>())
#else
			if (TooltipProperty->IsA<UStrProperty>())
#endif
			{
				if (const FString* ValuePtr = TooltipProperty->ContainerPtrToValuePtr<FString>(RowData))
				{
					NewPulldownRow.TooltipText = FText::FromString(*ValuePtr);
				}
			}
#if UE_4_25_OR_LATER
			else if (TooltipProperty->IsA<FNameProperty>())
#else
			else if (TooltipProperty->IsA<UNameProperty>())
#endif
			{
				if (const FName* ValuePtr = TooltipProperty->ContainerPtrToValuePtr<FName>(RowData))
				{
					NewPulldownRow.TooltipText = FText::FromName(*ValuePtr);
				}
			}
#if UE_4_25_OR_LATER
			else if (TooltipProperty->IsA<FTextProperty>())
#else
			else if (TooltipProperty->IsA<UTextProperty>())
#endif
			{
				if (const FText* ValuePtr = TooltipProperty->ContainerPtrToValuePtr<FText>(RowData))
				{
					NewPulldownRow.TooltipText = *ValuePtr;
				}
			}
		}

		if (TextColorProperty != nullptr)
		{
			FLinearColor TextColor(ForceInit);
			bool bExtracted = false;
			if (TextColorProperty->Struct == TBaseStructure<FColor>::Get())
			{
				if (const FColor* ValuePtr = TextColorProperty->ContainerPtrToValuePtr<FColor>(RowData))
				{
					TextColor = *ValuePtr;
					bExtracted = true;
				}
			}
			else if (TextColorProperty->Struct == TBaseStructure<FLinearColor>::Get())
			{
				if (const FLinearColor* ValuePtr = TextColorProperty->ContainerPtrToValuePtr<FLinearColor>(RowData))
				{
					TextColor = *ValuePtr;
					bExtracted = true;
				}
			}
			else if (TextColorProperty->Struct == FSlateColor::StaticStruct())
			{
				if (const FSlateColor* ValuePtr = TextColorProperty->ContainerPtrToValuePtr<FSlateColor>(RowData))
				{
					TextColor = ValuePtr->GetSpecifiedColor();
					bExtracted = true;
				}
			}

			// Match the existing semantics: only apply the color when the source alpha is fully opaque.
			if (bExtracted && FMath::IsNearlyEqual(TextColor.A, 1.f))
			{
				NewPulldownRow.DisplayTextColor = TextColor;
			}
		}

		PulldownRows.Add(NewPulldownRow);
	}

	return PulldownRows;
}

bool UDataTablePulldownListGenerator::HasSourceAsset_Implementation() const
{
	return true;
}

FString UDataTablePulldownListGenerator::GetSourceAssetName_Implementation() const
{
	return SourceDataTable.GetAssetName();
}

void UDataTablePulldownListGenerator::PreChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info)
{
	if (!IsValid(Changed) ||
		Changed != SourceDataTable.LoadSynchronous() ||
		Info != FDataTableEditorUtils::EDataTableChangeInfo::RowList
	)
	{
		return;
	}

	CapturePreChangeSelectedValues();

	// Blueprint functions are not available during routing post load.
	if (!FUObjectThreadContext::Get().IsRoutingPostLoad)
	{
		PreSourceDataTableModify();
	}
}

void UDataTablePulldownListGenerator::PostChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info)
{
	if (!IsValid(Changed) ||
		Changed != SourceDataTable.LoadSynchronous() ||
		Info != FDataTableEditorUtils::EDataTableChangeInfo::RowList
	)
	{
		return;
	}

	CommitPostChangeSelectedValues();

	// Blueprint functions are not available during routing post load.
	if (!FUObjectThreadContext::Get().IsRoutingPostLoad)
	{
		PostSourceDataTableModify();
	}

	VerifyDefaultValue();
}

TArray<FName> UDataTablePulldownListGenerator::CollectCurrentSelectedValues() const
{
	if (const UDataTable* DataTable = SourceDataTable.LoadSynchronous())
	{
		return DataTable->GetRowNames();
	}

	return {};
}

bool UDataTablePulldownListGenerator::FindTooltip(const UScriptStruct* RowStruct, uint8* RowData, FText& TooltipText) const
{
	check(IsValid(RowStruct));

	FString TooltipPropertyName = DefaultPulldownTooltipName;
	if (RowStruct->HasMetaData(*TooltipPropertyMeta))
	{
		TooltipPropertyName = RowStruct->GetMetaData(*TooltipPropertyMeta);
	}

#if UE_4_25_OR_LATER
	for (auto* Property : TFieldRange<FProperty>(RowStruct))
#else
	for (auto* Property : TFieldRange<UProperty>(RowStruct))
#endif
	{
		if (Property == nullptr)
		{
			continue;
		}

		if (Property->GetName() != TooltipPropertyName)
		{
			continue;	
		}

#if UE_4_25_OR_LATER
		if (Property->IsA<FStrProperty>())
#else
		if (Property->IsA<UStrProperty>())
#endif
		{
			if (const auto* ValuePtr = Property->ContainerPtrToValuePtr<FString>(RowData))
			{
				TooltipText = FText::FromString(*ValuePtr);
				return true;
			}
		}

#if UE_4_25_OR_LATER
		if (Property->IsA<FNameProperty>())
#else
		if (Property->IsA<UNameProperty>())
#endif
		{
			if (const auto* ValuePtr = Property->ContainerPtrToValuePtr<FName>(RowData))
			{
				TooltipText = FText::FromName(*ValuePtr);
				return true;
			}
		}

#if UE_4_25_OR_LATER
		if (Property->IsA<FTextProperty>())
#else
		if (Property->IsA<UTextProperty>())
#endif
		{
			if (const auto* ValuePtr = Property->ContainerPtrToValuePtr<FText>(RowData))
			{
				TooltipText = *ValuePtr;
				return true;
			}
		}
	}

	return false;
}

bool UDataTablePulldownListGenerator::FindTextColor(const UScriptStruct* RowStruct, uint8* RowData, FLinearColor& TextColor) const
{
	check(IsValid(RowStruct));

	FString TextColorPropertyName = DefaultPulldownTextColorName;
	if (RowStruct->HasMetaData(*TextColorPropertyMeta))
	{
		TextColorPropertyName = RowStruct->GetMetaData(*TextColorPropertyMeta);
	}

#if UE_4_25_OR_LATER
	for (auto* Property : TFieldRange<FStructProperty>(RowStruct))
#else
	for (auto* Property : TFieldRange<UStructProperty>(RowStruct))
#endif
	{
		if (Property == nullptr)
		{
			continue;
		}

		if (Property->GetName() != TextColorPropertyName)
		{
			continue;	
		}

		if (Property->Struct == TBaseStructure<FColor>::Get())
		{
			if (const auto* ValuePtr = Property->ContainerPtrToValuePtr<FColor>(RowData))
			{
				TextColor = *ValuePtr;
				break;
			}
		}

		if (Property->Struct == TBaseStructure<FLinearColor>::Get())
		{
			if (const auto* ValuePtr = Property->ContainerPtrToValuePtr<FLinearColor>(RowData))
			{
				TextColor = *ValuePtr;
				break;
			}
		}
		
		if (Property->Struct == FSlateColor::StaticStruct())
		{
			if (const auto* ValuePtr = Property->ContainerPtrToValuePtr<FSlateColor>(RowData))
			{
				TextColor = ValuePtr->GetSpecifiedColor();
				break;
			}
		}
	}

	return FMath::IsNearlyEqual(TextColor.A, 1.f);
}

TArray<FName> UDataTablePulldownListGenerator::GetDefaultValueOptions_Implementation() const
{
	if (!SourceDataTable.IsValid())
	{
		return {};
	}

	const TMap<FName, uint8*>& RowMap = SourceDataTable->GetRowMap();
	
	TArray<FName> RowNames;
	RowMap.GenerateKeyArray(RowNames);

	return RowNames;
}
