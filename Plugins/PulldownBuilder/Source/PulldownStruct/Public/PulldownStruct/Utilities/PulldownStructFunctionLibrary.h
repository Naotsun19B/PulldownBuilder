// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
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
	// Compares FPulldownStructBase::SelectedValue between pulldown structures of the same type and returns if the values are equal.
	UFUNCTION(BlueprintPure, CustomThunk, meta = (AutoCreateRefTerm = "Lhs,Rhs", BlueprintInternalUseOnly = true, CustomStructureParam = "Lhs,Rhs"))
	static bool Equal_PulldownStruct(const FPulldownStructBase& Lhs, const FPulldownStructBase& Rhs);
	static bool GenericEqual_PulldownStruct(
		const UScriptStruct* LhsType,
		const void* LhsValue,
		const UScriptStruct* RhsType,
		const void* RhsValue,
		bool& bReturnValue
	);
	DECLARE_FUNCTION(execEqual_PulldownStruct)
	{
		P_GET_STRUCT_REF(FPulldownStructBase, Lhs);
		const UScriptStruct* LhsType = nullptr;
#if BEFORE_UE_4_24
		if (const auto* LhsProperty = Cast<UStructProperty>(Stack.MostRecentProperty))
#else
		if (const auto* LhsProperty = CastField<FStructProperty>(Stack.MostRecentProperty))
#endif
		{
			LhsType = LhsProperty->Struct;
		}
		const void* LhsValue = &LhsTemp;
		
		P_GET_STRUCT_REF(FPulldownStructBase, Rhs);
		const UScriptStruct* RhsType = nullptr;
#if BEFORE_UE_4_24
		if (const auto* RhsProperty = Cast<UStructProperty>(Stack.MostRecentProperty))
#else
		if (const auto* RhsProperty = CastField<FStructProperty>(Stack.MostRecentProperty))
#endif
		{
			RhsType = RhsProperty->Struct;
		}
		const void* RhsValue = &RhsTemp;
		
		P_FINISH;
		bool bReturnValue = false;

		P_NATIVE_BEGIN;
		if (ensure(IsValid(LhsType) && LhsValue != nullptr && IsValid(RhsType) && RhsValue != nullptr))
		{
			bool bAreEachValueEqual = false;
			if (GenericEqual_PulldownStruct(LhsType, LhsValue, RhsType, RhsValue, bAreEachValueEqual))
			{
				bReturnValue = bAreEachValueEqual;
			}
		}
		P_NATIVE_END;

		*static_cast<bool*>(RESULT_PARAM) = bReturnValue;
	}

	// Compares FPulldownStructBase::SelectedValue between pulldown structures of the same type and returns if the values are not equal.
	UFUNCTION(BlueprintPure, CustomThunk, meta = (AutoCreateRefTerm = "Lhs,Rhs", BlueprintInternalUseOnly = true, CustomStructureParam = "Lhs,Rhs"))
	static bool NotEqual_PulldownStruct(const FPulldownStructBase& Lhs, const FPulldownStructBase& Rhs);
	static bool GenericNotEqual_PulldownStruct(
		const UScriptStruct* LhsType,
		const void* LhsValue,
		const UScriptStruct* RhsType,
		const void* RhsValue,
		bool& bReturnValue
	);
	DECLARE_FUNCTION(execNotEqual_PulldownStruct)
	{
		P_GET_STRUCT_REF(FPulldownStructBase, Lhs);
		const UScriptStruct* LhsType = nullptr;
#if BEFORE_UE_4_24
		if (const auto* LhsProperty = Cast<UStructProperty>(Stack.MostRecentProperty))
#else
		if (const auto* LhsProperty = CastField<FStructProperty>(Stack.MostRecentProperty))
#endif
		{
			LhsType = LhsProperty->Struct;
		}
		const void* LhsValue = &LhsTemp;
		
		P_GET_STRUCT_REF(FPulldownStructBase, Rhs);
		const UScriptStruct* RhsType = nullptr;
#if BEFORE_UE_4_24
		if (const auto* RhsProperty = Cast<UStructProperty>(Stack.MostRecentProperty))
#else
		if (const auto* RhsProperty = CastField<FStructProperty>(Stack.MostRecentProperty))
#endif
		{
			RhsType = RhsProperty->Struct;
		}
		const void* RhsValue = &RhsTemp;
		
		P_FINISH;
		bool bReturnValue = false;

		P_NATIVE_BEGIN;
		if (ensure(IsValid(LhsType) && LhsValue != nullptr && IsValid(RhsType) && RhsValue != nullptr))
		{
			bool bAreEachValueNotEqual = false;
			if (GenericNotEqual_PulldownStruct(LhsType, LhsValue, RhsType, RhsValue, bAreEachValueNotEqual))
			{
				bReturnValue = bAreEachValueNotEqual;
			}
		}
		P_NATIVE_END;

		*static_cast<bool*>(RESULT_PARAM) = bReturnValue;
	}
	
private:
	// Extract the FPulldownStructBase::SelectedValue value from the raw data of the pulldown structure.
	static bool TryGetSelectedValue(FName& SelectedValue, const UScriptStruct* Type, const void* Value);
};
