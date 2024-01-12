// Copyright 2021-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_Compare_PulldownStruct.h"
#include "K2Node_Equal_PulldownStruct.generated.h"

/**
 * A compare node that returns whether the values in FPulldownStructBase::SelectedValue are equal.
 */
UCLASS()
class PULLDOWNSTRUCTNODES_API UK2Node_Equal_PulldownStruct : public UK2Node_Compare_PulldownStruct
{
	GENERATED_BODY()

protected:
	// UK2Node_Compare_PulldownStruct interface.
	virtual FText GetCompareMethodName() const override;
	virtual FText GetCompareMethodOperator() const override;
	virtual FName GetFunctionName() const override;
	// End of UK2Node_Compare_PulldownStruct interface.
};
