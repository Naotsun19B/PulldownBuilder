// Copyright 2021-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "PulldownListGeneratorCacheDiffTest.generated.h"

/**
 * A test-only UPulldownListGeneratorBase whose CollectCurrentSelectedValues result is controlled by the
 * test through SetCurrentSelectedValuesForTest. Used to exercise the M-2 Template Method pair
 * CapturePreChangeSelectedValues / CommitPostChangeSelectedValues without needing a real source asset.
 */
UCLASS()
class UScriptedListGeneratorForTest : public UPulldownListGeneratorBase
{
	GENERATED_BODY()

public:
	// Replaces the snapshot CollectCurrentSelectedValues() will return on the next call.
	void SetCurrentSelectedValuesForTest(const TArray<FName>& InValues)
	{
		CurrentSelectedValuesForTest = InValues;
	}

	// Public wrappers for the protected Template Method members so the AutomationTest can drive them.
	void TriggerCapturePreChangeSelectedValuesForTest()
	{
		CapturePreChangeSelectedValues();
	}

	bool TriggerCommitPostChangeSelectedValuesForTest()
	{
		return CommitPostChangeSelectedValues();
	}

protected:
	// UPulldownListGeneratorBase interface.
	virtual TArray<FName> CollectCurrentSelectedValues() const override
	{
		return CurrentSelectedValuesForTest;
	}
	// End of UPulldownListGeneratorBase interface.

private:
	UPROPERTY(Transient)
	TArray<FName> CurrentSelectedValuesForTest;
};
