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

	// Returns whether the specified structure is already registered.
	static bool IsRegisteredPulldownStruct(const UScriptStruct* InStruct);
};
