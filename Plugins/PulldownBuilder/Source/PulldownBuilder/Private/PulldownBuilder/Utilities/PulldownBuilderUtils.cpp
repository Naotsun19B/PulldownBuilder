// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Types/PulldownRow.h"
#include "PulldownBuilder/Types/StructContainer.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "PulldownStruct/NativeLessPulldownStruct.h"
#include "EdGraphSchema_K2.h"
#include "Serialization/JsonSerializer.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Editor.h"
#include "Subsystems/AssetEditorSubsystem.h"
#if UE_4_26_OR_LATER
#include "AssetRegistry/IAssetRegistry.h"
#else
#include "AssetRegistryModule.h"
#include "IAssetRegistry.h"
#endif
#include "Modules/ModuleManager.h"
#include "ISettingsModule.h"
#include "PropertyEditorModule.h"

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

	bool FPulldownBuilderUtils::IsPulldownStruct(
		const UScriptStruct* InTestStruct,
		const bool bExcludeNativeLessPulldownStruct /* = true */
	)
	{
		const bool bBasedOnPulldownStructBase = IsChildStruct(FPulldownStructBase::StaticStruct(), InTestStruct);

		bool bBasedOnNativeLessPulldownStruct = false;
		if (bExcludeNativeLessPulldownStruct)
		{
			bBasedOnNativeLessPulldownStruct = IsNativeLessPulldownStruct(InTestStruct);
		}
		
		// Note: The return value of "UEdGraphSchema_K2::IsAllowableBlueprintVariableType" is true
		// because the struct that inherits any of the structures will be automatically added
		// "BlueprintType" unless "NotBlueprintType" and "BlueprintInternalUseOnly" is added in
		// the USTRUCT of the child struct.
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

	void FPulldownBuilderUtils::EnumeratePulldownContents(const TFunction<bool(UPulldownContents&)>& Callback)
	{
		const UClass* PulldownContentsClass = UPulldownContents::StaticClass();
		check(IsValid(PulldownContentsClass));
		
		IAssetRegistry* AssetRegistry = GetAssetRegistry();
		if (AssetRegistry == nullptr)
		{
			return;
		}

		TArray<FAssetData> AssetDataList;
#if UE_5_01_OR_LATER
		const FTopLevelAssetPath& ClassPathName = PulldownContentsClass->GetClassPathName();
		if (!AssetRegistry->GetAssetsByClass(ClassPathName, AssetDataList))
#else
		if (!AssetRegistry->GetAssetsByClass(PulldownContentsClass->GetFName(), AssetDataList))
#endif
		{
			return;
		}

		for (const auto& AssetData : AssetDataList)
		{
			if (auto* PulldownContents = Cast<UPulldownContents>(AssetData.GetAsset()))
			{
				Callback(*PulldownContents);
			}
		}
	}

	TArray<UPulldownContents*> FPulldownBuilderUtils::GetAllPulldownContents()
	{
		TArray<UPulldownContents*> PulldownContentsList;
		EnumeratePulldownContents([&](UPulldownContents& PulldownContents) -> bool
		{
			PulldownContentsList.Add(&PulldownContents);
			return true;
		});
		
		return PulldownContentsList;
	}

	UPulldownContents* FPulldownBuilderUtils::FindPulldownContentsByStruct(const UScriptStruct* InStruct)
	{
		UPulldownContents* FoundItem = nullptr;
		EnumeratePulldownContents([&](UPulldownContents& PulldownContents) -> bool
	    {
	        if (InStruct == PulldownContents.GetPulldownStructType().SelectedStruct)
	        {
        		FoundItem = &PulldownContents;
	            return false;
	        }
			
	        return true;
	    });

		return FoundItem;
	}

	UPulldownContents* FPulldownBuilderUtils::FindPulldownContentsByName(const FName& InName)
	{
		UPulldownContents* FoundItem = nullptr;
		EnumeratePulldownContents([&](UPulldownContents& PulldownContents) -> bool
		{
			if (InName == PulldownContents.GetFName())
			{
				FoundItem = &PulldownContents;
				return false;
			}
			
			return true;
		});

		return FoundItem;
	}

	bool FPulldownBuilderUtils::OpenPulldownContents(UPulldownContents* PulldownContents)
	{
		check(IsValid(GEditor) && IsValid(PulldownContents));

		if (auto* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
		{
			return AssetEditorSubsystem->OpenEditorForAsset(PulldownContents);
		}

		return false;
	}

	FPulldownRows FPulldownBuilderUtils::GetPulldownRowsFromStruct(
		const UScriptStruct* InStruct,
		const TArray<UObject*>& OuterObjects,
		const FStructContainer& StructInstance
	)
	{
		FPulldownRows PulldownRows;
		if (const UPulldownContents* FoundItem = FindPulldownContentsByStruct(InStruct))
		{
			PulldownRows = FoundItem->GetPulldownRows(OuterObjects, StructInstance);
		}

		return PulldownRows;
	}

	bool FPulldownBuilderUtils::IsRegisteredPulldownStruct(const UScriptStruct* InStruct)
	{
		bool bIsRegistered = false;
		EnumeratePulldownContents([&](const UPulldownContents& PulldownContents) -> bool
	    {
			if (InStruct == PulldownContents.GetPulldownStructType().SelectedStruct)
			{
				bIsRegistered = true;
				return false;
			}
			
	        return true;
	    });
	    
		return bIsRegistered;
	}

	bool FPulldownBuilderUtils::HasPulldownStructPostSerialize(const UScriptStruct* InStruct)
	{
		check(IsValid(InStruct));
		
		if (UScriptStruct::ICppStructOps* CppStructOps = InStruct->GetCppStructOps())
		{
			return CppStructOps->HasPostSerialize();
		}

		return false;
	}

	TMap<FString, FString> FPulldownBuilderUtils::StructStringToPropertyMap(const FString& StructString)
	{
		// Remove parentheses at both ends of the struct string and double quotes used in string types.
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

	TSharedPtr<FString> FPulldownBuilderUtils::StructStringToMemberValue(
		const FString& StructString, const FName& PropertyName) 
	{
		const TMap<FString, FString>& PropertiesMap = StructStringToPropertyMap(StructString);
		if (PropertiesMap.Contains(PropertyName.ToString()))
		{
			return MakeShared<FString>(*PropertiesMap[PropertyName.ToString()]);
		}
		
		return nullptr;
	}

	TSharedPtr<FString> FPulldownBuilderUtils::MemberValueToStructString(const FString& StructString, const FName& PropertyName, const FString& NewPropertyValue)
	{
		// If the value does not change, do nothing.
		const TSharedPtr<FString> OldPropertyValue = StructStringToMemberValue(StructString, PropertyName);
		if (!OldPropertyValue.IsValid() || NewPropertyValue == *OldPropertyValue)
		{
			return nullptr;
		}

		// Updates the value of the specified property.
		TMap<FString, FString> PropertiesMap = StructStringToPropertyMap(StructString);
		if (PropertiesMap.Contains(PropertyName.ToString()))
		{
			PropertiesMap[PropertyName.ToString()] = NewPropertyValue;
		}

		// Create a struct string from the property map.
		FString NewDefaultValue = TEXT("(");
		for (const auto& PropertyPair : PropertiesMap)
		{
			NewDefaultValue += FString::Printf(TEXT("%s=\"%s\","), *PropertyPair.Key, *PropertyPair.Value);
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
								PropertyValue = PropertyValue.Mid(1, PropertyValue.Len() - 1);
							}
							if (PropertyValue[PropertyValue.Len() - 1] == TEXT('"'))
							{
								PropertyValue = PropertyValue.Mid(0, PropertyValue.Len() - 1);
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
				JsonString = JsonString.Mid(0, JsonString.Len() - 1);
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

	UObject* FPulldownBuilderUtils::GetOuterAssetFromPin(const UEdGraphPin* Pin)
	{
		check(Pin != nullptr);
		
		UObject* CurrentObject = Pin->GetOuter();
		while (IsValid(CurrentObject))
		{
			if (CurrentObject->IsAsset())
			{
				return CurrentObject;
			}

			CurrentObject = CurrentObject->GetOuter();
		}

		return nullptr;
	}
	
	bool FPulldownBuilderUtils::GenerateStructContainerFromPin(const UEdGraphPin* Pin, FStructContainer& StructContainer)
	{
		check(Pin != nullptr);

		const auto* ScriptStruct = Cast<UScriptStruct>(Pin->PinType.PinSubCategoryObject);
		if (!IsValid(ScriptStruct))
		{
			return false;
		}

		// If the default value is empty, the raw data of the structure cannot be generated,
		// so set the default value in advance.
		const FString& DefaultValue = Pin->DefaultValue;
		if (DefaultValue.IsEmpty())
		{
			const_cast<FString&>(DefaultValue) = GenerateStructDefaultValueString(ScriptStruct);
		}
		
		auto* RawData = static_cast<uint8*>(FMemory::Malloc(ScriptStruct->GetStructureSize()));

		const bool bWasSuccessful = GetStructRawDataFromDefaultValueString(
			ScriptStruct,
			DefaultValue,
			RawData
		);
		if (bWasSuccessful)
		{
			StructContainer = FStructContainer(ScriptStruct, RawData);
		}

		FMemory::Free(RawData);

		return bWasSuccessful;
	}

	FString FPulldownBuilderUtils::GenerateStructDefaultValueString(const UScriptStruct* StructType)
	{
		check(IsValid(StructType));
		
		FString DefaultValueString = TEXT("(");	
#if UE_4_25_OR_LATER
		for (FProperty* Property : TFieldRange<FProperty>(StructType))
#else
		for (UProperty* Property : TFieldRange<UProperty>(StructType))
#endif
		{
#if UE_4_25_OR_LATER
			if (Property == nullptr)
#else
			if (!IsValid(Property))
#endif
			{
				continue;
			}

			DefaultValueString += FString::Printf(TEXT("%s=,"), *Property->GetName());
		}
		DefaultValueString[DefaultValueString.Len() - 1] = TEXT(')');

		return DefaultValueString;
	}

	FString FPulldownBuilderUtils::GetStructDefaultValueString(const UScriptStruct* StructType, const UEdGraphPin* Pin)
	{
		FString DefaultValue = GenerateStructDefaultValueString(StructType);
						
		const TSharedPtr<FPulldownRow> DefaultRow = GetDefaultRowFromPin(Pin);
		if (!DefaultRow.IsValid())
		{
			return DefaultValue;
		}

		const TSharedPtr<FString> ModifiedDefaultValue = MemberValueToStructString(
			DefaultValue,
			GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue),
			*DefaultRow->SelectedValue
		);
		if (!ModifiedDefaultValue.IsValid())
		{
			return *DefaultValue;
		}
						
		return *ModifiedDefaultValue;
	}

	TSharedPtr<FPulldownRow> FPulldownBuilderUtils::GetDefaultRowFromPin(const UEdGraphPin* Pin)
	{
		FStructContainer StructContainer;
		if (!GenerateStructContainerFromPin(Pin, StructContainer))
		{
			return nullptr;
		}

		const FPulldownRows PulldownRows = GetPulldownRowsFromStruct(
			StructContainer.GetScriptStruct(),
			TArray<UObject*>{ GetOuterAssetFromPin(Pin) },
			StructContainer
		);

		return PulldownRows.GetDefaultRow();
	}

	IAssetRegistry* FPulldownBuilderUtils::GetAssetRegistry()
	{
#if UE_4_26_OR_LATER
		return IAssetRegistry::Get();
#else
		if (const auto* AssetRegistryModule = FModuleManager::LoadModulePtr<FAssetRegistryModule>(TEXT("AssetRegistry")))
		{
			return &AssetRegistryModule->Get();
		}

		return nullptr;
#endif
	}

	FPropertyEditorModule& FPulldownBuilderUtils::GetPropertyEditorModule()
	{
		return FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	}

	ISettingsModule& FPulldownBuilderUtils::GetSettingsModule()
	{
		return FModuleManager::LoadModuleChecked<ISettingsModule>(TEXT("Settings"));
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
		ISettingsModule& SettingsModule = GetSettingsModule();
		SettingsModule.RegisterSettings(
			ContainerName,
			CategoryName,
			SectionName,
			DisplayName,
			Description,
			SettingsObject
		);
	}

	void FPulldownBuilderUtils::UnregisterSettings(
		const FName& ContainerName,
		const FName& CategoryName,
		const FName& SectionName
	)
	{
		ISettingsModule& SettingsModule = GetSettingsModule();
		SettingsModule.UnregisterSettings(
			ContainerName,
			CategoryName,
			SectionName
		);
	}
}
