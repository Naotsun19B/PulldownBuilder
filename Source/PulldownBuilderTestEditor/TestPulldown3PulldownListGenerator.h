﻿// Copyright 2021-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "TestPulldown3PulldownListGenerator.generated.h"

/**
 * A sample that generates a list based on properties other than SelectedValue of the pull-down struct.
 */
UCLASS(DisplayName = "Test Pulldown 3 Pulldown List Generator", meta = (FilterPulldownStructTypes = "TestPulldown3"))
class UTestPulldown3PulldownListGenerator : public UPulldownListGeneratorBase
{
	GENERATED_BODY()

public:
	// UPulldownListGeneratorBase interface.
	virtual TArray<TSharedPtr<FPulldownRow>> GetPulldownRows(
		const TArray<UObject*>& OuterObjects,
		const FStructContainer& StructInstance
	) const override;
	// End of UPulldownListGeneratorBase interface.
};
