// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownBuilderTestEditor/PulldownStructDetailCoordinatorTest.h"
#include "Misc/AutomationTest.h"
#include "PulldownBuilder/Assets/PulldownContentsDelegates.h"

/**
 * Regression / behaviour test for H-3.
 *
 * Pre-fix, UPulldownContents itself drove FPulldownStructDetail::Register and Unregister from PostLoad,
 * PreEditChange, PostEditChangeProperty, BeginDestroy, and HandleOnPackageReloaded. Post-fix that
 * responsibility lives in FPulldownStructDetailCoordinator, which listens to the new lifecycle delegates
 * (OnPulldownStructTypeChangePending / OnPulldownStructTypeChangeCommitted) and the existing load
 * delegates (OnPulldownContentsLoaded). UPulldownContents now only broadcasts the lifecycle delegates.
 *
 * This test pins:
 *   - Broadcasting OnPulldownContentsLoaded for a PulldownContents whose StructType is fresh causes the
 *     coordinator to broadcast OnDetailCustomizationRegistered for that content.
 *   - Broadcasting OnPulldownStructTypeChangePending causes the coordinator to broadcast
 *     OnDetailCustomizationUnregistered.
 *   - Broadcasting OnPulldownStructTypeChangeCommitted re-broadcasts OnDetailCustomizationRegistered.
 *
 * Also indirectly verifies that the OnDetailCustomizationUnregistered delegate has the correct
 * declared type (regression for C-2) -- binding a lambda with the documented signature must compile.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPulldownBuilderPulldownStructDetailCoordinatorTest,
	"PulldownBuilder.Utilities.PulldownStructDetailCoordinator.DelegateChain",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FPulldownBuilderPulldownStructDetailCoordinatorTest::RunTest(const FString& Parameters)
{
	using namespace PulldownBuilder;

	// 1. Set up a test PulldownContents with a unique StructType.
	UTestablePulldownContentsForCoordinatorTest* TestContents =
		NewObject<UTestablePulldownContentsForCoordinatorTest>(GetTransientPackage());
	TestNotNull(TEXT("NewObject<UTestablePulldownContentsForCoordinatorTest> succeeded"), TestContents);
	if (TestContents == nullptr)
	{
		return false;
	}
	TestContents->AddToRoot();

	UScriptStruct* TestStructType = FCoordinatorTestStruct::StaticStruct();
	TestNotNull(TEXT("FCoordinatorTestStruct::StaticStruct() returns non-null"), TestStructType);
	TestContents->SetPulldownStructTypeForTest(TestStructType);
	TestTrue(TEXT("PulldownStructType is valid after setter"), TestContents->GetPulldownStructType().IsValid());

	// 2. Wire up listeners that count the customization broadcasts targeting our test contents.
	int32 RegisteredCount = 0;
	int32 UnregisteredCount = 0;

	const FDelegateHandle RegisteredHandle = FPulldownContentsDelegates::OnDetailCustomizationRegistered.AddLambda(
		[&](const UPulldownContents* RegisteredPulldownContents)
		{
			if (RegisteredPulldownContents == TestContents)
			{
				++RegisteredCount;
			}
		}
	);
	// Binding with the correct one-param signature here also serves as a compile-time guard for C-2.
	const FDelegateHandle UnregisteredHandle = FPulldownContentsDelegates::OnDetailCustomizationUnregistered.AddLambda(
		[&](const UPulldownContents* UnregisteredPulldownContents)
		{
			if (UnregisteredPulldownContents == TestContents)
			{
				++UnregisteredCount;
			}
		}
	);

	// 3. Simulate the lifecycle.

	// 3a. OnPulldownContentsLoaded -> coordinator registers.
	FPulldownContentsDelegates::OnPulldownContentsLoaded.Broadcast(TestContents);
	TestEqual(
		TEXT("After OnPulldownContentsLoaded, coordinator must broadcast OnDetailCustomizationRegistered exactly once"),
		RegisteredCount, 1
	);
	TestEqual(
		TEXT("After OnPulldownContentsLoaded, no Unregistered broadcasts yet"),
		UnregisteredCount, 0
	);

	// 3b. OnPulldownStructTypeChangePending -> coordinator unregisters.
	FPulldownContentsDelegates::OnPulldownStructTypeChangePending.Broadcast(TestContents);
	TestEqual(
		TEXT("After OnPulldownStructTypeChangePending, coordinator must broadcast OnDetailCustomizationUnregistered"),
		UnregisteredCount, 1
	);

	// 3c. OnPulldownStructTypeChangeCommitted -> coordinator re-registers.
	FPulldownContentsDelegates::OnPulldownStructTypeChangeCommitted.Broadcast(TestContents);
	TestEqual(
		TEXT("After OnPulldownStructTypeChangeCommitted, coordinator must broadcast OnDetailCustomizationRegistered again"),
		RegisteredCount, 2
	);

	// 4. Clean up: broadcast a final ChangePending so the coordinator removes our test StructType from
	//    its internal map and from FPulldownStructDetail's static registry, leaving the editor session
	//    in the same state as before the test.
	FPulldownContentsDelegates::OnPulldownStructTypeChangePending.Broadcast(TestContents);
	TestEqual(
		TEXT("Final cleanup unregister was observed"),
		UnregisteredCount, 2
	);

	FPulldownContentsDelegates::OnDetailCustomizationRegistered.Remove(RegisteredHandle);
	FPulldownContentsDelegates::OnDetailCustomizationUnregistered.Remove(UnregisteredHandle);

	TestContents->RemoveFromRoot();

	return true;
}
