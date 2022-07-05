﻿// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Types/PulldownRow.h"
#include "PulldownBuilder/Types/StructContainer.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "PulldownStruct/NativeLessPulldownStruct.h"
#include "UObject/UObjectIterator.h"
#include "EdGraphSchema_K2.h"
#include "Serialization/JsonSerializer.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Modules/ModuleManager.h"
#include "ISettingsModule.h"

namespace PulldownBuilder
{
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
		for (UPulldownContents* PulldownContents : TObjectRange<UPulldownContents>())
		{
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

	TArray<TSharedPtr<FPulldownRow>> FPulldownBuilderUtils::GetPulldownRowsFromStruct(
		const UScriptStruct* InStruct,
		const TArray<UObject*>& OuterObjects,
		const FStructContainer& StructInstance
	)
	{
		TArray<TSharedPtr<FPulldownRow>> PulldownRows;
		if (const UPulldownContents* FoundItem = FindPulldownContentsByStruct(InStruct))
		{
			PulldownRows = FoundItem->GetPulldownRows(OuterObjects, StructInstance);
		}

		return PulldownRows;
	}

	TArray<TSharedPtr<FPulldownRow>> FPulldownBuilderUtils::GetEmptyPulldownRows()
	{
		TArray<TSharedPtr<FPulldownRow>> EmptySelectableValues;
		EmptySelectableValues.Add(MakeShared<FPulldownRow>());
		return EmptySelectableValues;
	}

	bool FPulldownBuilderUtils::IsRegisteredPulldownStruct(const UScriptStruct* InStruct)
	{
		bool bIsRegistered = false;
		EnumeratePulldownContents([&](const UPulldownContents* PulldownContents) -> bool
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
		for (UProperty* Property : TFieldRange<UProperty>(InStruct))
#else
		for (FProperty* Property : TFieldRange<FProperty>(InStruct))
#endif
		{
			if (Property == nullptr)
			{
				continue;
			}

			DefaultValueString += FString::Printf(TEXT("%s=,"), *Property->GetName());
		}
		DefaultValueString[DefaultValueString.Len() - 1] = TEXT(')');

		return DefaultValueString;
	}

	TMap<FString, FString> FPulldownBuilderUtils::StructStringToPropertyMap(const FString& StructString)
	{
		// Remove parentheses at both ends of the structure string and double quotes used in string types.
		FString PropertyValues = StructString;
		PropertyValues = PropertyValues.Replace(TEXT("("), TEXT(""));
		PropertyValues = PropertyValues.Replace(TEXT(")"), TEXT(""));
		PropertyValues = PropertyValues.Replace(TEXT("\""), TEXT(""));

		// Create a map of variable names and values by splitting properties.
		TArray<FString> ParsedPropertyValues;
		PropertyValues.ParseIntoArray(ParsedPropertyValues, TEXT(","));
		
		TMap<FString, FString> PropertiesMap;
		for (const auto& ParsedPropertyValue : ParsedPropertyValues)
		{
			FString VariableName;
			FString VariableValue;
			ParsedPropertyValue.Split(TEXT("="), &VariableName, &VariableValue);

			PropertiesMap.Add(VariableName, VariableValue);
		}

		return PropertiesMap;
	}

	TSharedPtr<FName> FPulldownBuilderUtils::StructStringToMemberValue(const FString& StructString, const FName& PropertyName) 
	{
		const TMap<FString, FString>& PropertiesMap = StructStringToPropertyMap(StructString);
		if (PropertiesMap.Contains(PropertyName.ToString()))
		{
			return MakeShared<FName>(*PropertiesMap[PropertyName.ToString()]);
		}
		
		return nullptr;
	}

	TSharedPtr<FString> FPulldownBuilderUtils::MemberValueToStructString(const FString& StructString, const FName& PropertyName, const FName& NewPropertyValue)
	{
		// If the value does not change, do nothing.
		const TSharedPtr<FName> OldPropertyValue = StructStringToMemberValue(StructString, PropertyName);
		if (!OldPropertyValue.IsValid() || NewPropertyValue == *OldPropertyValue)
		{
			return nullptr;
		}

		// Updates the value of the specified property.
		TMap<FString, FString> PropertiesMap = StructStringToPropertyMap(StructString);
		if (PropertiesMap.Contains(PropertyName.ToString()))
		{
			PropertiesMap[PropertyName.ToString()] = NewPropertyValue.ToString();
		}

		// Create a structure string from the property map.
		FString NewDefaultValue = TEXT("(");
		for (const auto& PropertyPair : PropertiesMap)
		{
			NewDefaultValue += FString::Printf(TEXT("%s=%s,"), *PropertyPair.Key, *PropertyPair.Value);
		}
		NewDefaultValue[NewDefaultValue.Len() - 1] = TEXT(')');
		
		return MakeShared<FString>(NewDefaultValue);
	}

	bool FPulldownBuilderUtils::GetStructRawDataFromDefaultValueString(
		const UScriptStruct* StructType,
		const FString& DefaultValue,
		uint8*& RawData
	)
	{
		TSharedPtr<FJsonObject> JsonObject;
		{
			FString JsonString;
			{
				const FString TrimmedDefaultValue = DefaultValue.Mid(1, DefaultValue.Len() - 2);
				TMap<FString, FString> PropertyNameToValue;
				
				TArray<FString> PropertyStrings;
				TrimmedDefaultValue.ParseIntoArray(PropertyStrings, TEXT(","));
				PropertyNameToValue.Reserve(PropertyStrings.Num());
				
				for (const auto& PropertyString : PropertyStrings)
				{
					FString PropertyName;
					FString PropertyValue;
					if (PropertyString.Split(TEXT("="), &PropertyName, &PropertyValue))
					{
						if (PropertyValue.Len() > 0)
						{
							if (PropertyValue[0] == TEXT('"'))
							{
								PropertyValue.MidInline(1, PropertyValue.Len() - 1);
							}
							if (PropertyValue[PropertyValue.Len() - 1] == TEXT('"'))
							{
								PropertyValue.MidInline(0, PropertyValue.Len() - 1);
							}
						}
						PropertyNameToValue.Add(PropertyName, PropertyValue);
					}
				}
				if (PropertyNameToValue.Num() == 0)
				{
					return false;
				}

				JsonString += TEXT("{");
				for (const auto& Pair : PropertyNameToValue)
				{
					const FString PropertyName = Pair.Key;
					const FString PropertyValue = Pair.Value;
					JsonString += FString::Printf(TEXT("\"%s\" : \"%s\","), *PropertyName, *PropertyValue);
				}
				JsonString.MidInline(0, JsonString.Len() - 1);
				JsonString += TEXT("}");
			}

			const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
			if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
			{
				return false;
			}
		}
		if (!JsonObject.IsValid())
		{
			return false;
		}

		if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), StructType, RawData))
		{
			return false;
		}

		return (RawData != nullptr);
	}

	ISettingsModule* FPulldownBuilderUtils::GetSettingsModule()
	{
		return FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	}

	void FPulldownBuilderUtils::RegisterSettings(
		const FName& ContainerName,
		const FName& CategoryName,
		const FName& SectionName,
		const FText& DisplayName,
		const FText& Description,
		const TWeakObjectPtr<UObject>& SettingsObject
	)
	{
		if (ISettingsModule* SettingsModule = GetSettingsModule())
		{
			SettingsModule->RegisterSettings(
				ContainerName,
				CategoryName,
				SectionName,
				DisplayName,
				Description,
				SettingsObject
			);
		}
	}

	void FPulldownBuilderUtils::UnregisterSettings(
		const FName& ContainerName,
		const FName& CategoryName,
		const FName& SectionName
	)
	{
		if (ISettingsModule* SettingsModule = GetSettingsModule())
		{
			SettingsModule->UnregisterSettings(
				ContainerName,
				CategoryName,
				SectionName
			);
		}
	}
}
