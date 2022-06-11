// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UPulldownContents;
struct FPulldownRow;

namespace PulldownBuilder
{
	/**
	 * Utility function library class that defines the processes widely used in this module.
	 */
	class PULLDOWNBUILDER_API FPulldownBuilderUtils
	{
	public:
		// Checks if the specified structure inherits the specified structure.
		static bool IsChildStruct(const UScriptStruct* InSuperStruct, const UScriptStruct* InTestStruct);
	
		// Checks if the specified structure is inherits FPulldownStructBase.
		static bool IsPulldownStruct(const UScriptStruct* InTestStruct);

		// Checks if the specified structure is FNativeLessPulldownStruct or a structure that inherits FNativeLessPulldownStruct.
		static bool IsNativeLessPulldownStruct(const UScriptStruct* InTestStruct);

		// Scans all PulldownContents present in the Content Browser.
		static void EnumeratePulldownContents(const TFunction<bool(UPulldownContents*)>& Callback);
	
		// Gets all PulldownContents that exist on the Content Browser.
		static TArray<UPulldownContents*> GetAllPulldownContents();

		// Finds PulldownContents that has the specified structure set.
		// If not found, returns nullptr.
		static UPulldownContents* FindPulldownContentsByStruct(const UScriptStruct* InStruct);

		// Finds PulldownContents with the specified name.
		// If not found, returns nullptr.
		static UPulldownContents* FindPulldownContentsByName(const FName& InName);

		// Get the list of character strings to be displayed in the pull-down menu from PulldownContents
		// obtained by FindPulldownContentsByStruct.
		static TArray<TSharedPtr<FPulldownRow>> GetPulldownRowsFromStruct(const UScriptStruct* InStruct);

		// Generates "None" only display strings.
		static TArray<TSharedPtr<FPulldownRow>> GetEmptyPulldownRows();
	
		// Returns whether the specified structure is already registered.
		static bool IsRegisteredPulldownStruct(const UScriptStruct* InStruct);

		// Generates a string that represents the default value of the structure.
		static FString GenerateStructDefaultValueString(const UScriptStruct* InStruct);

		// Gets the value of the string state structure as a map of variable names and values.
		static TMap<FString, FString> StructStringToPropertyMap(const FString& StructString);
	
		// Gets or sets the value of a variable with the specified name.
		// If specify a property name that does not exist and get it, nullptr is returned.
		static TSharedPtr<FName> StructStringToMemberValue(const FString& StructString, const FName& PropertyName);
		static TSharedPtr<FString> MemberValueToStructString(const FString& StructString, const FName& PropertyName, const FName& NewPropertyValue);
	};
}
