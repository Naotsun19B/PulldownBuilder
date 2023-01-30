// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/K2Nodes/K2Node_NotEqual_PulldownStruct.h"
#include "Kismet/KismetMathLibrary.h"

#define LOCTEXT_NAMESPACE "K2Node_NotEqual_PulldownStruct"

FText UK2Node_NotEqual_PulldownStruct::GetCompareMethodName() const
{
	return LOCTEXT("CompareMethodName", "NotEqual");
}

FText UK2Node_NotEqual_PulldownStruct::GetCompareMethodOperator() const
{
	return LOCTEXT("CompareMethodOperator", "!=");
}

FName UK2Node_NotEqual_PulldownStruct::GetFunctionName() const
{
	return GET_FUNCTION_NAME_CHECKED(UKismetMathLibrary, NotEqual_NameName);
}

#undef LOCTEXT_NAMESPACE
