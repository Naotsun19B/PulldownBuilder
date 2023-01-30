// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/StructContainerFunctionLibrary.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

bool UStructContainerFunctionLibrary::Get_StructContainer(const FStructContainer& Target, int32& StructData)
{
	unimplemented();
	return false;
}

bool UStructContainerFunctionLibrary::GenericGet_StructContainer(
	const FStructContainer& Target,
	const UScriptStruct* StructType,
	void* StructRawData
)
{
	if(!Target.IsValid() || !IsValid(StructType) || Target.GetScriptStruct() != StructType)
	{
		StructType->ClearScriptStruct(StructRawData);
		return false;
	}
		
	StructType->CopyScriptStruct(StructRawData, Target.GetMemory());
	return true;
}

DEFINE_FUNCTION(UStructContainerFunctionLibrary::execGet_StructContainer)
{
	P_GET_STRUCT_REF(FStructContainer, Target);
	Stack.MostRecentProperty = nullptr;
#if BEFORE_UE_4_24
	Stack.StepCompiledIn<UStructProperty>(nullptr);
	const auto* StructProperty = Cast<UStructProperty>(Stack.MostRecentProperty);
#else
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	const auto* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
#endif
	void* StructRawData = Stack.MostRecentPropertyAddress;

	P_FINISH;
	bool bReturnValue = false;
		
	P_NATIVE_BEGIN;
	if (ensure(StructProperty != nullptr && StructRawData != nullptr))
	{
		bReturnValue = GenericGet_StructContainer(Target, StructProperty->Struct, StructRawData);
	}
	P_NATIVE_END;

	*static_cast<bool*>(RESULT_PARAM) = bReturnValue;
}
