// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UPulldownContents;

/**
 * Utility function library class that defines the processes widely used in this module.
 */
class PULLDOWNBUILDER_API FPulldownBuilderUtils
{
public:
	// Checks if the specified structure is FPulldownStructBase or a structure that inherits FPulldownStructBase.
	static bool IsPulldownStruct(const UScriptStruct* InStruct);

	// Scans all Pulldown Contents present in the Content Browser.
	static void EnumeratePulldownContents(const TFunction<bool(UPulldownContents*)>& Callback);
	
	// Gets all Pulldown Contents that exist on the Content Browser.
	static TArray<UPulldownContents*> GetAllPulldownContents();

	// Finds Pulldown Contents that has the specified structure set.
	// If not found, returns nullptr.
	static UPulldownContents* FindPulldownContentsByStruct(const UScriptStruct* InStruct);

	// Get the list of character strings to be displayed in the pull-down menu from Pulldown Contents
	// obtained by FindPulldownContentsByStruct.
	static TArray<TSharedPtr<FString>> GetDisplayStringsFromStruct(const UScriptStruct* InStruct);
	
	// Returns whether the specified structure is already registered.
	static bool IsRegisteredPulldownStruct(const UScriptStruct* InStruct);
};
