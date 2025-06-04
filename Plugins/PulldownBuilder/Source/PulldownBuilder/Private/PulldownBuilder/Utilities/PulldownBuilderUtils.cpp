// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownBuilder/Assets/PulldownContentsAsyncLoader.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Types/PulldownRow.h"
#include "PulldownBuilder/Types/StructContainer.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "PulldownStruct/NativeLessPulldownStruct.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "EdGraphSchema_K2.h"
#include "Editor.h"
#include "Subsystems/AssetEditorSubsystem.h"

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

	TArray<UPulldownContents*> FPulldownBuilderUtils::GetAllPulldownContents()
	{
		TArray<UPulldownContents*> PulldownContentsList;
		FPulldownContentsAsyncLoader::EnumeratePulldownContents(
			[&](UPulldownContents& PulldownContents) -> bool
			{
				PulldownContentsList.Add(&PulldownContents);
				return true;
			}
		);
		
		return PulldownContentsList;
	}

	UPulldownContents* FPulldownBuilderUtils::FindPulldownContentsByStruct(const UScriptStruct* InStruct)
	{
		UPulldownContents* FoundItem = nullptr;
		FPulldownContentsAsyncLoader::EnumeratePulldownContents(
			[&](UPulldownContents& PulldownContents) -> bool
		    {
		        if (InStruct == PulldownContents.GetPulldownStructType().SelectedStruct)
		        {
        			FoundItem = &PulldownContents;
		            return false;
		        }
				
		        return true;
		    }
	    );

		return FoundItem;
	}

	UPulldownContents* FPulldownBuilderUtils::FindPulldownContentsByName(const FName& InName)
	{
		UPulldownContents* FoundItem = nullptr;
		FPulldownContentsAsyncLoader::EnumeratePulldownContents(
			[&](UPulldownContents& PulldownContents) -> bool
			{
				if (InName == PulldownContents.GetFName())
				{
					FoundItem = &PulldownContents;
					return false;
				}
				
				return true;
			}
		);

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
		if (const UPulldownContents* FoundPulldownContents = FindPulldownContentsByStruct(InStruct))
		{
			PulldownRows = FoundPulldownContents->GetPulldownRows(OuterObjects, StructInstance);
		}

		return PulldownRows;
	}

	bool FPulldownBuilderUtils::IsRegisteredPulldownStruct(const UScriptStruct* InStruct)
	{
		bool bIsRegistered = false;
		FPulldownContentsAsyncLoader::EnumeratePulldownContents(
			[&](const UPulldownContents& PulldownContents) -> bool
		    {
				if (InStruct == PulldownContents.GetPulldownStructType().SelectedStruct)
				{
					bIsRegistered = true;
					return false;
				}
				
		        return true;
		    }
	    );
	    
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
			if (!ParsedPropertyValue.Split(TEXT("="), &VariableName, &VariableValue))
			{
				continue;
			}

			PropertiesMap.Add(VariableName, VariableValue);
		}

		return PropertiesMap;
	}

	TSharedPtr<FString> FPulldownBuilderUtils::StructStringToMemberValue(const FString& StructString, const FName& PropertyName) 
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

		auto* ScriptStruct = Cast<UScriptStruct>(Pin->PinType.PinSubCategoryObject);
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
		
		const TMap<FString, FString> PropertyNameToValue = StructStringToPropertyMap(DefaultValue);
		if (PropertyNameToValue.Num() == 0)
		{
			return false;
		}

		auto* RawData = static_cast<uint8*>(FMemory::Malloc(ScriptStruct->GetStructureSize()));
		ScriptStruct->InitializeStruct(RawData);
		
		bool bWasSuccessful = false;

		if (ScriptStruct->ImportText(*DefaultValue, RawData, nullptr, PPF_None, GLog, GetNameSafe(ScriptStruct)) != nullptr)
		{
			StructContainer = FStructContainer(ScriptStruct, RawData);
			bWasSuccessful = true;
		}

		ScriptStruct->DestroyStruct(RawData);
		FMemory::Free(RawData);

		return bWasSuccessful;
	}

	FString FPulldownBuilderUtils::GenerateStructDefaultValueString(const UScriptStruct* StructType)
	{
		check(IsValid(StructType));

		auto* RawData = static_cast<uint8*>(FMemory::Malloc(StructType->GetStructureSize()));
		StructType->InitializeDefaultValue(RawData);

		FString DefaultValueString;
		StructType->ExportText(DefaultValueString, RawData, RawData, nullptr, PPF_SerializedAsImportText, nullptr);

		StructType->DestroyStruct(RawData);
		FMemory::Free(RawData);
		
		return DefaultValueString;
	}

	FString FPulldownBuilderUtils::GetStructDefaultValueString(const FString& DefaultValueString, const UEdGraphPin* Pin)
	{
		const TSharedPtr<FPulldownRow> DefaultRow = GetDefaultRowFromPin(Pin);
		if (!DefaultRow.IsValid())
		{
			return DefaultValueString;
		}

		const TSharedPtr<FString> ModifiedDefaultValue = MemberValueToStructString(
			DefaultValueString,
			GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue),
			*DefaultRow->SelectedValue
		);
		if (!ModifiedDefaultValue.IsValid())
		{
			return *DefaultValueString;
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
}
