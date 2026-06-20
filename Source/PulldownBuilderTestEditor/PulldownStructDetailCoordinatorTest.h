// Copyright 2021-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownStructDetailCoordinatorTest.generated.h"

/**
 * A USTRUCT unique to FPulldownStructDetailCoordinatorTest. Using a test-only type guarantees no real
 * PulldownContents asset has registered a detail customization for it, so FPulldownStructDetail::Register
 * does not collide with anything pre-existing in the editor session.
 */
USTRUCT()
struct FCoordinatorTestStruct
{
	GENERATED_BODY()
};

/**
 * A UPulldownContents subclass that exposes PulldownStructType setters to the test.
 * UPulldownContents marks the field as `protected`, so a subclass-friendly accessor is required.
 */
UCLASS()
class UTestablePulldownContentsForCoordinatorTest : public UPulldownContents
{
	GENERATED_BODY()

public:
	// Test-only setter for PulldownStructType.SelectedStruct. Required because the field is protected on the base.
	void SetPulldownStructTypeForTest(UScriptStruct* InStructType)
	{
		PulldownStructType = FPulldownStructType(InStructType);
	}
};
