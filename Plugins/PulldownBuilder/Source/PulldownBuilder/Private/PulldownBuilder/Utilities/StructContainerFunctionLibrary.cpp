// Copyright 2021-2024 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/StructContainerFunctionLibrary.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(StructContainerFunctionLibrary)
#endif

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
#if UE_4_25_OR_LATER
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	const auto* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
#else
	Stack.StepCompiledIn<UStructProperty>(nullptr);
	const auto* StructProperty = Cast<UStructProperty>(Stack.MostRecentProperty);
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
