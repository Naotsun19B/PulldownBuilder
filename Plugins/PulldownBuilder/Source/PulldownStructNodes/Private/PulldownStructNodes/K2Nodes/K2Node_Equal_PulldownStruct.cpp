// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/K2Nodes/K2Node_Equal_PulldownStruct.h"
#include "PulldownStruct/Utilities/PulldownStructFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "K2Node_Equal_PulldownStruct"

FText UK2Node_Equal_PulldownStruct::GetCompareMethodName() const
{
	return LOCTEXT("CompareMethodName", "Equal");
}

FText UK2Node_Equal_PulldownStruct::GetCompareMethodOperator() const
{
	return LOCTEXT("CompareMethodOperator", "==");
}

void UK2Node_Equal_PulldownStruct::GetFunction(FName& FunctionName, TSubclassOf<UObject>& MemberParentClass) const
{
	FunctionName = GET_FUNCTION_NAME_CHECKED(UPulldownStructFunctionLibrary, Equal_PulldownStruct);
	MemberParentClass = UPulldownStructFunctionLibrary::StaticClass();
}

#undef LOCTEXT_NAMESPACE
