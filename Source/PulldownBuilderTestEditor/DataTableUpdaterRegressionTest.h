// Copyright 2021-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PulldownBuilder/RowNameUpdaters/DataTableUpdater.h"
#include "PulldownBuilderTest/TestPulldown.h"
#include "DataTableUpdaterRegressionTest.generated.h"

/**
 * A row type used only by FDataTableUpdaterRegressionTest. It contains multiple FStructProperty fields
 * (FActorName extends FPulldownStructBase) so that the pre-fix behaviour and the post-fix behaviour can
 * be distinguished by counting UpdateMemberVariables invocations:
 *
 *   - Pre-fix (buggy) C-3: for each row, UpdateMemberVariables is called once per FStructProperty field
 *                          (here: 3 times per row).
 *   - Post-fix C-3        : for each row, UpdateMemberVariables is called exactly once.
 */
USTRUCT()
struct FDataTableUpdaterRegressionRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY()
	FActorName SlotA;

	UPROPERTY()
	FActorName SlotB;

	UPROPERTY()
	FActorName SlotC;
};

/**
 * A test-only UDataTableUpdater whose UpdateMemberVariables override counts how many times it is invoked
 * for rows of FDataTableUpdaterRegressionRow. Other DataTable rows in the editor session are intentionally
 * ignored so this counter is independent of unrelated DataTables in memory.
 */
UCLASS()
class UCountingDataTableUpdaterForTest : public UDataTableUpdater
{
	GENERATED_BODY()

public:
	// Counter incremented every time UpdateMemberVariables is called with our test row type.
	int32 UpdateMemberVariablesCallCount = 0;

	// Public wrapper around the protected UpdateRowNamesInternal so the test entry point can drive it.
	void TriggerUpdateRowNamesInternalForTest(
		UPulldownContents* PulldownContents,
		const FName& PreChangeSelectedValue,
		const FName& PostChangeSelectedValue
	)
	{
		UpdateRowNamesInternal(PulldownContents, PreChangeSelectedValue, PostChangeSelectedValue);
	}

protected:
	// URowNameUpdaterBase interface.
	virtual bool UpdateMemberVariables(
		const UStruct* Struct,
		void* ContainerPtr,
		UPulldownContents* PulldownContents,
		const FName& PreChangeSelectedValue,
		const FName& PostChangeSelectedValue
	) override
	{
		if (Struct == FDataTableUpdaterRegressionRow::StaticStruct())
		{
			++UpdateMemberVariablesCallCount;
		}
		// Return false to ensure the surrounding UEditorLoadingAndSavingUtils::SavePackages path is not triggered.
		return false;
	}
	// End of URowNameUpdaterBase interface.
};
