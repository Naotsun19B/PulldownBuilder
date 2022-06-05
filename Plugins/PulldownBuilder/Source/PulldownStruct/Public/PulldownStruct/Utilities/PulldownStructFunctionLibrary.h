// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "PulldownStructFunctionLibrary.generated.h"

/**
 * A class that defines a blueprint node for pulldown struct.
 */
UCLASS()
class PULLDOWNSTRUCT_API UPulldownStructFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Compares FPulldownStructBase::SelectedValue between pulldown structures of the same type and returns the result.
	UFUNCTION(BlueprintPure, CustomThunk, meta = (AutoCreateRefTerm = "Lhs, Rhs", BlueprintInternalUseOnly = true, CustomStructureParam = "Lhs, Rhs"))
	static bool Equal_PulldownStruct(const FPulldownStructBase& Lhs, const FPulldownStructBase& Rhs);
	static bool GenericEqual_PulldownStruct(
		const UScriptStruct* LhsType,
		const void* LhsValue,
		const UScriptStruct* RhsType,
		const void* RhsValue
	);
	DECLARE_FUNCTION(execEqual_PulldownStruct)
	{
		P_GET_STRUCT_REF(FPulldownStructBase, Lhs);
		Stack.StepCompiledIn<FStructProperty>(nullptr);
		const FStructProperty* LhsStructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
		const void* LhsValue = Stack.MostRecentPropertyAddress;

		P_GET_STRUCT_REF(FPulldownStructBase, Rhs);
		Stack.StepCompiledIn<FStructProperty>(nullptr);
		const FStructProperty* RhsStructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
		const void* RhsValue = Stack.MostRecentPropertyAddress;
		
		P_FINISH;
		bool bReturnValue = false;

		P_NATIVE_BEGIN;
		if (ensure(LhsStructProperty != nullptr && LhsValue != nullptr && RhsStructProperty != nullptr && RhsValue != nullptr))
		{
			const UScriptStruct* LhsType = LhsStructProperty->Struct;
			const UScriptStruct* RhsType = RhsStructProperty->Struct;
			if (ensure(IsValid(LhsType) && IsValid(RhsType)))
			{
				bReturnValue = GenericEqual_PulldownStruct(LhsType, LhsValue, RhsType, RhsValue);
			}
		}
		P_NATIVE_END;

		*static_cast<bool*>(RESULT_PARAM) = bReturnValue;
	}

private:
	// Extract the FPulldownStructBase::SelectedValue value from the raw data of the pulldown structure.
	static bool TryGetSelectedValue(FName& SelectedValue, const UScriptStruct* Type, const void* Value);
};
