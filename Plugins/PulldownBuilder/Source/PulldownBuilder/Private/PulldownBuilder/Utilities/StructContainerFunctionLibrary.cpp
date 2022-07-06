// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/StructContainerFunctionLibrary.h"

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
