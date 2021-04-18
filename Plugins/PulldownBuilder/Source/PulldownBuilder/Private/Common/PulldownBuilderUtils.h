// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Utility function library class that defines the processes widely used in this module.
 */
class PULLDOWNBUILDER_API FPulldownBuilderUtils
{
public:
	// Checks if the specified structure is FPulldownStructBase or a structure that inherits FPulldownStructBase.
	static bool IsPulldownStruct(const UScriptStruct* InStruct);
};
