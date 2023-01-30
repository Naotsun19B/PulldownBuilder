// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/K2Nodes/K2Node_Equal_PulldownStruct.h"
#include "Kismet/KismetMathLibrary.h"

#define LOCTEXT_NAMESPACE "K2Node_Equal_PulldownStruct"

FText UK2Node_Equal_PulldownStruct::GetCompareMethodName() const
{
	return LOCTEXT("CompareMethodName", "Equal");
}

FText UK2Node_Equal_PulldownStruct::GetCompareMethodOperator() const
{
	return LOCTEXT("CompareMethodOperator", "==");
}

FName UK2Node_Equal_PulldownStruct::GetFunctionName() const
{
	return GET_FUNCTION_NAME_CHECKED(UKismetMathLibrary, EqualEqual_NameName);
}

#undef LOCTEXT_NAMESPACE
