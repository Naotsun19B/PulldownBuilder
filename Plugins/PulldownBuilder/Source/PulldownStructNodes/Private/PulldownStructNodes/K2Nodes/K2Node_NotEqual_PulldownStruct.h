﻿// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_Compare_PulldownStruct.h"
#include "K2Node_NotEqual_PulldownStruct.generated.h"

/**
 * Compares FPulldownStructBase::SelectedValue between pull-down structures of the same type and returns if the values are not equal.
 */
UCLASS()
class PULLDOWNSTRUCTNODES_API UK2Node_NotEqual_PulldownStruct : public UK2Node_Compare_PulldownStruct
{
	GENERATED_BODY()

protected:
	// UK2Node_Compare_PulldownStruct interface.
	virtual FText GetCompareMethodName() const override;
	virtual FText GetCompareMethodOperator() const override;
	virtual FName GetFunctionName() const override;
	// End of UK2Node_Compare_PulldownStruct interface.
};
