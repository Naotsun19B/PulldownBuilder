// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/K2Nodes/K2Node_NotEqual_PulldownStruct.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "Kismet/KismetMathLibrary.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_NotEqual_PulldownStruct)
#endif

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

FName UK2Node_NotEqual_PulldownStruct::GetPulldownSourceFunctionName() const
{
	// For NotEqual, PulldownSource also needs to be compared with NotEqual,
	// so that the reducer (BooleanOR) can correctly express
	// "values differ when either the source or the selected value differs".
	return GET_FUNCTION_NAME_CHECKED(UKismetMathLibrary, NotEqual_NameName);
}

FName UK2Node_NotEqual_PulldownStruct::GetReducerFunctionName() const
{
	// For NotEqual, the two sub-results must be combined with OR:
	// (SourceNotEqual) OR (SelectedValueNotEqual) == !((SourceEqual) AND (SelectedValueEqual)).
	return GET_FUNCTION_NAME_CHECKED(UKismetMathLibrary, BooleanOR);
}

#undef LOCTEXT_NAMESPACE
