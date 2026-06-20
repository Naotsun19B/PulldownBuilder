// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownBuilderTestEditor/DataTableUpdaterRegressionTest.h"
#include "Misc/AutomationTest.h"

/**
 * Regression test for C-3.
 *
 * The pre-fix UDataTableUpdater::UpdateRowNamesInternal contained an outer TFieldRange<FStructProperty>
 * loop that did nothing with its iteration variable -- it just re-ran the inner UpdateMemberVariables
 * call once per FStructProperty field of the row struct. With the fix the outer loop is gone, so each
 * row is processed exactly once.
 *
 * This test pins the post-fix behaviour by:
 *   1. Standing up a transient UDataTable whose row struct has 3 FStructProperty fields.
 *   2. Populating 3 rows.
 *   3. Running UCountingDataTableUpdaterForTest::TriggerUpdateRowNamesInternalForTest, which counts
 *      UpdateMemberVariables calls matching that row struct.
 *   4. Asserting the counter equals 3 (= row count). Pre-fix the counter would have been 9 (= rows * fields).
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPulldownBuilderDataTableUpdaterCallCountTest,
	"PulldownBuilder.RowNameUpdaters.DataTableUpdater.SingleCallPerRow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FPulldownBuilderDataTableUpdaterCallCountTest::RunTest(const FString& Parameters)
{
	// Build a transient DataTable rooted to GC so it survives the test scope.
	UDataTable* TestDataTable = NewObject<UDataTable>(GetTransientPackage());
	TestNotNull(TEXT("NewObject<UDataTable> succeeded"), TestDataTable);
	if (TestDataTable == nullptr)
	{
		return false;
	}
	TestDataTable->AddToRoot();

	UCountingDataTableUpdaterForTest* CountingUpdater = NewObject<UCountingDataTableUpdaterForTest>(GetTransientPackage());
	TestNotNull(TEXT("NewObject<UCountingDataTableUpdaterForTest> succeeded"), CountingUpdater);
	if (CountingUpdater == nullptr)
	{
		TestDataTable->RemoveFromRoot();
		return false;
	}
	CountingUpdater->AddToRoot();

	// Populate the row struct + three rows.
	TestDataTable->RowStruct = FDataTableUpdaterRegressionRow::StaticStruct();

	const FDataTableUpdaterRegressionRow EmptyRow;
	TestDataTable->AddRow(FName(TEXT("Row1")), EmptyRow);
	TestDataTable->AddRow(FName(TEXT("Row2")), EmptyRow);
	TestDataTable->AddRow(FName(TEXT("Row3")), EmptyRow);
	TestEqual(TEXT("Test DataTable has the expected row count"), TestDataTable->GetRowNames().Num(), 3);

	// Drive UpdateRowNamesInternal. PulldownContents is intentionally nullptr because the counting override
	// never dereferences it.
	CountingUpdater->TriggerUpdateRowNamesInternalForTest(
		nullptr,
		FName(TEXT("Pre")),
		FName(TEXT("Post"))
	);

	// Post-fix: exactly one UpdateMemberVariables call per row. Pre-fix would have produced 3 * 3 = 9 calls.
	TestEqual(
		TEXT("UpdateMemberVariables must be invoked exactly once per row (regression for C-3 multi-loop bug)"),
		CountingUpdater->UpdateMemberVariablesCallCount,
		3
	);

	CountingUpdater->RemoveFromRoot();
	TestDataTable->RemoveFromRoot();
	return true;
}
