// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class IAssetRegistry;
class ISettingsModule;
class FPropertyEditorModule;
class UScriptStruct;
class UEdGraphPin;
class UPulldownContents;
struct FPulldownRow;
struct FStructContainer;

namespace PulldownBuilder
{
	/**
	 * A utility class that defines the processes widely used in this plugin.
	 */
	class PULLDOWNBUILDER_API FPulldownBuilderUtils
	{
	public:
		// Returns whether the specified struct inherits from the specified struct.
		static bool IsChildStruct(const UScriptStruct* InSuperStruct, const UScriptStruct* InTestStruct);
	
		// Returns whether the specified struct inherits from FPulldownStructBase.
		static bool IsPulldownStruct(
			const UScriptStruct* InTestStruct,
			const bool bExcludeNativeLessPulldownStruct = true
		);

		// Returns whether the specified struct is a FNativeLessPulldownStruct or a struct that inherits from it.
		static bool IsNativeLessPulldownStruct(const UScriptStruct* InTestStruct);

		// Enumerates all PulldownContents present in the Content Browser.
		static void EnumeratePulldownContents(const TFunction<bool(UPulldownContents&)>& Callback);
	
		// Gets all PulldownContents that exist on the Content Browser.
		static TArray<UPulldownContents*> GetAllPulldownContents();

		// Finds PulldownContents that has the specified struct set.
		static UPulldownContents* FindPulldownContentsByStruct(const UScriptStruct* InStruct);

		// Finds PulldownContents with the specified name.
		static UPulldownContents* FindPulldownContentsByName(const FName& InName);

		// Opens the asset editor for the specified PulldownContents.
		// Returns whether the asset editor was opened.
		static bool OpenPulldownContents(UPulldownContents* PulldownContents);
		
		// Gets the list of character strings to be displayed in the pull-down menu from PulldownContents obtained by FindPulldownContentsByStruct.
		static TArray<TSharedPtr<FPulldownRow>> GetPulldownRowsFromStruct(
			const UScriptStruct* InStruct,
			const TArray<UObject*>& OuterObjects,
			const FStructContainer& StructInstance
		);

		// Generates "None" only display strings.
		static TArray<TSharedPtr<FPulldownRow>> GetEmptyPulldownRows();
	
		// Returns whether the specified struct is already registered.
		static bool IsRegisteredPulldownStruct(const UScriptStruct* InStruct);

		// Returns whether the specified structure implements PostSerialize.
		static bool HasPulldownStructPostSerialize(const UScriptStruct* InStruct);
		
		// Generates a string that represents the default value of the struct.
		static FString GenerateStructDefaultValueString(const UScriptStruct* InStruct);

		// Gets the value of the string state struct as a map of variable names and values.
		static TMap<FString, FString> StructStringToPropertyMap(const FString& StructString);
	
		// Gets or sets the value of a variable with the specified name.
		// If specify a property name that does not exist and get it, nullptr is returned.
		static TSharedPtr<FName> StructStringToMemberValue(const FString& StructString, const FName& PropertyName);
		static TSharedPtr<FString> MemberValueToStructString(const FString& StructString, const FName& PropertyName, const FName& NewPropertyValue);

		// Generates the raw data of the struct from the string of the default value of the pin.
		static bool GetStructRawDataFromDefaultValueString(
			const UScriptStruct* StructType,
			const FString& DefaultValue,
			uint8*& RawData
		);

		// Searches and returns the outer assets of the pin recursively.
		static UObject* GetOuterAssetFromPin(const UEdGraphPin* Pin);

		// Creates an FStructContainer from the data of the property pointed to by this pin.
		static bool GenerateStructContainerFromPin(const UEdGraphPin* Pin, FStructContainer& StructContainer);

		// Returns an asset registry from the asset registry module.
		static IAssetRegistry* GetAssetRegistry();
		
		// Returns a reference to the property editor module.
		static FPropertyEditorModule& GetPropertyEditorModule();
		
		// Returns a module that registers editor preferences etc. added by the plugin.
		static ISettingsModule& GetSettingsModule();

		// Registers custom setting class in the editor preferences etc.
		static void RegisterSettings(
			const FName& ContainerName,
			const FName& CategoryName,
			const FName& SectionName,
			const FText& DisplayName,
			const FText& Description,
			const TWeakObjectPtr<UObject>& SettingsObject
		);

		// Unregisters the custom setting class registered in the editor preference etc.
		static void UnregisterSettings(
			const FName& ContainerName,
			const FName& CategoryName,
			const FName& SectionName
		);
	};
}
