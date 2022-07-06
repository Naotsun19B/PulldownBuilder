// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PulldownBuilder/Types/StructContainer.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
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
	DECLARE_FUNCTION(execGet_StructContainer)
	{
		P_GET_STRUCT_REF(FStructContainer, Target);
		Stack.MostRecentProperty = nullptr;
#if BEFORE_UE_4_24
		Stack.StepCompiledIn<UStructProperty>(nullptr);
#else
		Stack.StepCompiledIn<FStructProperty>(nullptr);
#endif
		void* StructRawData = Stack.MostRecentPropertyAddress;
		const auto* ValueStructProp =
#if BEFORE_UE_4_24
			Cast<UStructProperty>(Stack.MostRecentProperty);
#else
			CastField<FStructProperty>(Stack.MostRecentProperty);
#endif

		P_FINISH;
		bool bReturnValue = false;
		
		P_NATIVE_BEGIN;
		if (ValueStructProp != nullptr)
		{
			const UScriptStruct* StructType = ValueStructProp->Struct;
			if (IsValid(StructType) && StructRawData != nullptr)
			{
				bReturnValue = GenericGet_StructContainer(Target, StructType, StructRawData);
			}
		}
		P_NATIVE_END;

		*static_cast<bool*>(RESULT_PARAM) = bReturnValue;
	}
};
