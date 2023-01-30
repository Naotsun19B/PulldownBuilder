// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PulldownBuilder/Types/StructContainer.h"
#include "StructContainerFunctionLibrary.generated.h"

/**
 * A class that defines a blueprint node for FStructContainer.
 */
UCLASS()
class PULLDOWNBUILDER_API UStructContainerFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Returns the data of the structure stored in the container.
	UFUNCTION(BlueprintPure, CustomThunk, meta = (BlueprintInternalUseOnly = true, CustomStructureParam = "StructData"))
	static bool Get_StructContainer(const FStructContainer& Target, int32& StructData);
	static bool GenericGet_StructContainer(
		const FStructContainer& Target,
		const UScriptStruct* StructType,
		void* StructRawData
	);
	DECLARE_FUNCTION(execGet_StructContainer);
};
