// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Templates/SharedPointer.h"
#include "PulldownBuilder/DetailCustomizations/PulldownStructDetail.h"
#include "PulldownBuilder/Types/PulldownRow.h"
#include "PulldownBuilder/Types/PulldownRows.h"

namespace PulldownBuilderLookupCacheTest
{
	/**
	 * A test-only subclass that exposes the M-3 lookup-cache plumbing of FPulldownStructDetail
	 * (SelectableValues, SelectableValuesByName, RebuildSelectableValuesLookup, FindSelectableValueByName).
	 * The base members are protected; wrappers below give the AutomationTest a direct handle.
	 *
	 * IPropertyTypeCustomization's CustomizeHeader / CustomizeChildren are intentionally not called here:
	 * this test only exercises the in-memory lookup-cache invariants.
	 */
	class FTestablePulldownStructDetail : public PulldownBuilder::FPulldownStructDetail
	{
	public:
		FPulldownRows& AccessSelectableValuesForTest()
		{
			return SelectableValues;
		}

		const TMap<FString, TSharedPtr<FPulldownRow>>& AccessSelectableValuesByNameForTest() const
		{
			return SelectableValuesByName;
		}

		void TriggerRebuildSelectableValuesLookupForTest()
		{
			RebuildSelectableValuesLookup();
		}

		TSharedPtr<FPulldownRow> TriggerFindSelectableValueByNameForTest(const FName& InName) const
		{
			return FindSelectableValueByName(InName);
		}
	};
}

/**
 * M-3 regression for FPulldownStructDetail::FindSelectableValueByName / RebuildSelectableValuesLookup.
 *
 * Pre-fix the lookup performed a linear FindByPredicate over SelectableValues every call. The fix
 * introduces a TMap<FString, TSharedPtr<FPulldownRow>> mirror (SelectableValuesByName) that
 * RebuildSelectableValuesLookup populates so FindSelectableValueByName can resolve in O(1).
 *
 * This test pins:
 *   1. Before any rebuild, the cache is empty -- FindSelectableValueByName falls back to the linear
 *      scan and still returns the correct result.
 *   2. After a rebuild, the cache size equals SelectableValues.Num() and the lookup returns the
 *      matching row by SelectedValue.
 *   3. A miss returns nullptr.
 *   4. A second rebuild after SelectableValues is replaced reflects the new content (no stale entries).
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPulldownBuilderSelectableValuesLookupCacheTest,
	"PulldownBuilder.DetailCustomizations.PulldownStructDetail.SelectableValuesLookupCache",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FPulldownBuilderSelectableValuesLookupCacheTest::RunTest(const FString& Parameters)
{
	using namespace PulldownBuilderLookupCacheTest;

	TSharedRef<FTestablePulldownStructDetail> Detail = MakeShared<FTestablePulldownStructDetail>();

	// Seed SelectableValues. FPulldownRows always carries an initial "None" row, so adding three real rows
	// gives us SelectableValues.Num() == 4 (None + Apple + Banana + Cherry).
	FPulldownRows& Values = Detail->AccessSelectableValuesForTest();
	Values.Add(FPulldownRow(TEXT("Apple")));
	Values.Add(FPulldownRow(TEXT("Banana")));
	Values.Add(FPulldownRow(TEXT("Cherry")));
	TestEqual(TEXT("SelectableValues should hold one None row plus the three test rows"), Values.Num(), 4);

	// 1. Before RebuildSelectableValuesLookup, the cache is empty so FindSelectableValueByName falls back
	//    to the linear scan. The result must still be correct.
	TestEqual(
		TEXT("Cache is empty before the first rebuild"),
		Detail->AccessSelectableValuesByNameForTest().Num(), 0
	);

	const TSharedPtr<FPulldownRow> LinearScanResult = Detail->TriggerFindSelectableValueByNameForTest(FName(TEXT("Banana")));
	TestTrue(TEXT("Fallback linear scan must locate Banana even without the cache"), LinearScanResult.IsValid());
	if (LinearScanResult.IsValid())
	{
		TestEqual(
			TEXT("Linear-scan fallback returns the row with the requested SelectedValue"),
			LinearScanResult->SelectedValue, FString(TEXT("Banana"))
		);
	}

	// 2. After rebuild, the cache mirrors SelectableValues 1:1.
	Detail->TriggerRebuildSelectableValuesLookupForTest();
	TestEqual(
		TEXT("After rebuild the lookup-cache size matches SelectableValues.Num()"),
		Detail->AccessSelectableValuesByNameForTest().Num(), Values.Num()
	);

	const TSharedPtr<FPulldownRow> CachedResult = Detail->TriggerFindSelectableValueByNameForTest(FName(TEXT("Cherry")));
	TestTrue(TEXT("Cached lookup must locate Cherry"), CachedResult.IsValid());
	if (CachedResult.IsValid())
	{
		TestEqual(
			TEXT("Cached lookup returns the row with the requested SelectedValue"),
			CachedResult->SelectedValue, FString(TEXT("Cherry"))
		);
	}

	// 3. A miss returns nullptr.
	const TSharedPtr<FPulldownRow> MissingResult = Detail->TriggerFindSelectableValueByNameForTest(FName(TEXT("Durian")));
	TestFalse(TEXT("Lookup for a non-existent SelectedValue returns nullptr"), MissingResult.IsValid());

	// 4. Replace SelectableValues, rebuild, and confirm no stale entries leak through.
	Values.Reset();
	Values.Add(FPulldownRow(TEXT("Elderberry")));
	Detail->TriggerRebuildSelectableValuesLookupForTest();
	TestEqual(
		TEXT("After Reset+Add+Rebuild, cache size matches the new SelectableValues content"),
		Detail->AccessSelectableValuesByNameForTest().Num(), Values.Num()
	);

	const TSharedPtr<FPulldownRow> RefreshedResult = Detail->TriggerFindSelectableValueByNameForTest(FName(TEXT("Elderberry")));
	TestTrue(TEXT("Refreshed cache resolves the new row"), RefreshedResult.IsValid());

	const TSharedPtr<FPulldownRow> StaleResult = Detail->TriggerFindSelectableValueByNameForTest(FName(TEXT("Apple")));
	TestFalse(TEXT("Old entries must not survive a Reset+Rebuild"), StaleResult.IsValid());

	return true;
}
