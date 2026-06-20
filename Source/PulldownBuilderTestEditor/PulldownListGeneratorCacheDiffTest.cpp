// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownBuilderTestEditor/PulldownListGeneratorCacheDiffTest.h"
#include "PulldownBuilderTestEditor/PulldownStructDetailCoordinatorTest.h"
#include "Misc/AutomationTest.h"
#include "PulldownBuilder/Assets/PulldownContentsDelegates.h"

namespace PulldownBuilderCacheDiffTest
{
	/**
	 * Helper that spawns the test generator parented to a transient UPulldownContents so that
	 * GetTypedOuter<UPulldownContents>() inside Notify* succeeds and the diff is actually broadcast.
	 *
	 * Returned objects are AddToRoot'd; caller is responsible for RemoveFromRoot.
	 */
	struct FFixture
	{
		UTestablePulldownContentsForCoordinatorTest* Owner = nullptr;
		UScriptedListGeneratorForTest* Generator = nullptr;

		bool Setup(FAutomationTestBase& Test)
		{
			Owner = NewObject<UTestablePulldownContentsForCoordinatorTest>(GetTransientPackage());
			if (Owner == nullptr)
			{
				Test.AddError(TEXT("Failed to NewObject<UTestablePulldownContentsForCoordinatorTest>"));
				return false;
			}
			Owner->AddToRoot();

			Generator = NewObject<UScriptedListGeneratorForTest>(Owner);
			if (Generator == nullptr)
			{
				Test.AddError(TEXT("Failed to NewObject<UScriptedListGeneratorForTest>"));
				Owner->RemoveFromRoot();
				return false;
			}
			Generator->AddToRoot();

			Test.TestEqual(
				TEXT("Generator's typed outer should be the owning PulldownContents (so Notify* can broadcast)"),
				static_cast<UPulldownContents*>(Generator->GetTypedOuter<UPulldownContents>()),
				static_cast<UPulldownContents*>(Owner)
			);
			return true;
		}

		void Teardown()
		{
			if (Generator != nullptr)
			{
				Generator->RemoveFromRoot();
				Generator = nullptr;
			}
			if (Owner != nullptr)
			{
				Owner->RemoveFromRoot();
				Owner = nullptr;
			}
		}
	};

	/** Container for the broadcast counters that each test installs. */
	struct FCountingListener
	{
		int32 AddedCount = 0;
		int32 RemovedCount = 0;
		int32 RenamedCount = 0;
		int32 SourceChangedCount = 0;

		FName LastAddedSelectedValue = NAME_None;
		FName LastRemovedSelectedValue = NAME_None;
		FName LastRenamedPreChangeSelectedValue = NAME_None;
		FName LastRenamedPostChangeSelectedValue = NAME_None;

		FDelegateHandle AddedHandle;
		FDelegateHandle RemovedHandle;
		FDelegateHandle RenamedHandle;
		FDelegateHandle SourceChangedHandle;

		void Bind(UPulldownContents* OwnerFilter)
		{
			using namespace PulldownBuilder;

			AddedHandle = FPulldownContentsDelegates::OnPulldownRowAdded.AddLambda(
				[this, OwnerFilter](UPulldownContents* InContents, const FName& InAdded)
				{
					if (InContents == OwnerFilter)
					{
						++AddedCount;
						LastAddedSelectedValue = InAdded;
					}
				}
			);
			RemovedHandle = FPulldownContentsDelegates::OnPulldownRowRemoved.AddLambda(
				[this, OwnerFilter](UPulldownContents* InContents, const FName& InRemoved)
				{
					if (InContents == OwnerFilter)
					{
						++RemovedCount;
						LastRemovedSelectedValue = InRemoved;
					}
				}
			);
			RenamedHandle = FPulldownContentsDelegates::OnPulldownRowRenamed.AddLambda(
				[this, OwnerFilter](UPulldownContents* InContents, const FName& InPre, const FName& InPost)
				{
					if (InContents == OwnerFilter)
					{
						++RenamedCount;
						LastRenamedPreChangeSelectedValue = InPre;
						LastRenamedPostChangeSelectedValue = InPost;
					}
				}
			);
			SourceChangedHandle = FPulldownContentsDelegates::OnPulldownContentsSourceChanged.AddLambda(
				[this, OwnerFilter](UPulldownContents* InContents)
				{
					if (InContents == OwnerFilter)
					{
						++SourceChangedCount;
					}
				}
			);
		}

		void Unbind()
		{
			using namespace PulldownBuilder;

			FPulldownContentsDelegates::OnPulldownRowAdded.Remove(AddedHandle);
			FPulldownContentsDelegates::OnPulldownRowRemoved.Remove(RemovedHandle);
			FPulldownContentsDelegates::OnPulldownRowRenamed.Remove(RenamedHandle);
			FPulldownContentsDelegates::OnPulldownContentsSourceChanged.Remove(SourceChangedHandle);
		}
	};
}

/**
 * M-2 regression: when the list grows from N entries to N+1 entries, CommitPostChangeSelectedValues
 * must broadcast OnPulldownRowAdded for each new entry. NotifyPulldownRowChanged's add-branch is the
 * specific code path under test.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPulldownBuilderListGeneratorCacheDiffAddTest,
	"PulldownBuilder.ListGenerators.PulldownListGeneratorBase.CommitDiff.Added",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FPulldownBuilderListGeneratorCacheDiffAddTest::RunTest(const FString& Parameters)
{
	using namespace PulldownBuilderCacheDiffTest;

	FFixture Fixture;
	if (!Fixture.Setup(*this))
	{
		return false;
	}

	FCountingListener Listener;
	Listener.Bind(Fixture.Owner);

	// Pre-state: [Apple, Banana]. Snapshot it.
	Fixture.Generator->SetCurrentSelectedValuesForTest({ TEXT("Apple"), TEXT("Banana") });
	Fixture.Generator->TriggerCapturePreChangeSelectedValuesForTest();

	// Post-state: [Apple, Banana, Cherry]. Commit should report exactly one Add for "Cherry".
	Fixture.Generator->SetCurrentSelectedValuesForTest({ TEXT("Apple"), TEXT("Banana"), TEXT("Cherry") });
	const bool bWasNotified = Fixture.Generator->TriggerCommitPostChangeSelectedValuesForTest();

	TestTrue(TEXT("Commit must report bWasNotified=true when an addition is detected"), bWasNotified);
	TestEqual(TEXT("OnPulldownRowAdded must fire exactly once for the new entry"), Listener.AddedCount, 1);
	TestEqual(TEXT("OnPulldownRowAdded must report the new SelectedValue"), Listener.LastAddedSelectedValue, FName(TEXT("Cherry")));
	TestEqual(TEXT("No removals expected on grow"), Listener.RemovedCount, 0);
	TestEqual(TEXT("No renames expected on grow"), Listener.RenamedCount, 0);

	// After commit the cached baseline should equal post-state. A second commit (no further change) must report no diff.
	Listener.AddedCount = 0;
	const bool bWasSecondNotified = Fixture.Generator->TriggerCommitPostChangeSelectedValuesForTest();
	TestFalse(TEXT("Second commit with unchanged state must not report bWasNotified=true"), bWasSecondNotified);
	TestEqual(TEXT("Second commit must broadcast no additional Add"), Listener.AddedCount, 0);

	Listener.Unbind();
	Fixture.Teardown();
	return true;
}

/**
 * M-2 regression: when the list shrinks, CommitPostChangeSelectedValues must broadcast OnPulldownRowRemoved
 * for each removed entry.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPulldownBuilderListGeneratorCacheDiffRemovedTest,
	"PulldownBuilder.ListGenerators.PulldownListGeneratorBase.CommitDiff.Removed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FPulldownBuilderListGeneratorCacheDiffRemovedTest::RunTest(const FString& Parameters)
{
	using namespace PulldownBuilderCacheDiffTest;

	FFixture Fixture;
	if (!Fixture.Setup(*this))
	{
		return false;
	}

	FCountingListener Listener;
	Listener.Bind(Fixture.Owner);

	Fixture.Generator->SetCurrentSelectedValuesForTest({ TEXT("Apple"), TEXT("Banana"), TEXT("Cherry") });
	Fixture.Generator->TriggerCapturePreChangeSelectedValuesForTest();

	Fixture.Generator->SetCurrentSelectedValuesForTest({ TEXT("Apple"), TEXT("Banana") });
	const bool bWasNotified = Fixture.Generator->TriggerCommitPostChangeSelectedValuesForTest();

	TestTrue(TEXT("Commit must report bWasNotified=true when a removal is detected"), bWasNotified);
	TestEqual(TEXT("OnPulldownRowRemoved must fire exactly once"), Listener.RemovedCount, 1);
	TestEqual(TEXT("OnPulldownRowRemoved must report the removed SelectedValue"), Listener.LastRemovedSelectedValue, FName(TEXT("Cherry")));
	TestEqual(TEXT("No additions expected on shrink"), Listener.AddedCount, 0);
	TestEqual(TEXT("No renames expected on shrink"), Listener.RenamedCount, 0);

	Listener.Unbind();
	Fixture.Teardown();
	return true;
}

/**
 * M-2 regression: when the list count is unchanged but a position differs, CommitPostChangeSelectedValues
 * must broadcast OnPulldownRowRenamed for each differing position.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPulldownBuilderListGeneratorCacheDiffRenamedTest,
	"PulldownBuilder.ListGenerators.PulldownListGeneratorBase.CommitDiff.Renamed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FPulldownBuilderListGeneratorCacheDiffRenamedTest::RunTest(const FString& Parameters)
{
	using namespace PulldownBuilderCacheDiffTest;

	FFixture Fixture;
	if (!Fixture.Setup(*this))
	{
		return false;
	}

	FCountingListener Listener;
	Listener.Bind(Fixture.Owner);

	// Pre: [Apple, Banana, Cherry]. Post: [Apple, Berry, Cherry]. Same count, position 1 differs.
	Fixture.Generator->SetCurrentSelectedValuesForTest({ TEXT("Apple"), TEXT("Banana"), TEXT("Cherry") });
	Fixture.Generator->TriggerCapturePreChangeSelectedValuesForTest();

	Fixture.Generator->SetCurrentSelectedValuesForTest({ TEXT("Apple"), TEXT("Berry"), TEXT("Cherry") });
	const bool bWasNotified = Fixture.Generator->TriggerCommitPostChangeSelectedValuesForTest();

	TestTrue(TEXT("Commit must report bWasNotified=true when a rename is detected"), bWasNotified);
	TestEqual(TEXT("OnPulldownRowRenamed must fire exactly once for the differing position"), Listener.RenamedCount, 1);
	TestEqual(TEXT("OnPulldownRowRenamed must report the pre-change name"), Listener.LastRenamedPreChangeSelectedValue, FName(TEXT("Banana")));
	TestEqual(TEXT("OnPulldownRowRenamed must report the post-change name"), Listener.LastRenamedPostChangeSelectedValue, FName(TEXT("Berry")));
	TestEqual(TEXT("No additions expected on same-count rename"), Listener.AddedCount, 0);
	TestEqual(TEXT("No removals expected on same-count rename"), Listener.RemovedCount, 0);

	Listener.Unbind();
	Fixture.Teardown();
	return true;
}

/**
 * M-2 regression: CommitPostChangeSelectedValues must refresh the cached baseline so that a subsequent
 * commit with no further changes is a no-op (returns false, broadcasts nothing).
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPulldownBuilderListGeneratorCacheDiffBaselineRefreshTest,
	"PulldownBuilder.ListGenerators.PulldownListGeneratorBase.CommitDiff.BaselineRefresh",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FPulldownBuilderListGeneratorCacheDiffBaselineRefreshTest::RunTest(const FString& Parameters)
{
	using namespace PulldownBuilderCacheDiffTest;

	FFixture Fixture;
	if (!Fixture.Setup(*this))
	{
		return false;
	}

	FCountingListener Listener;
	Listener.Bind(Fixture.Owner);

	// Seed: [A, B]. Capture. Change to [A, B, C]. Commit -> adds reported, cache becomes [A, B, C].
	Fixture.Generator->SetCurrentSelectedValuesForTest({ TEXT("A"), TEXT("B") });
	Fixture.Generator->TriggerCapturePreChangeSelectedValuesForTest();
	Fixture.Generator->SetCurrentSelectedValuesForTest({ TEXT("A"), TEXT("B"), TEXT("C") });
	(void)Fixture.Generator->TriggerCommitPostChangeSelectedValuesForTest();
	TestEqual(TEXT("First commit reports the addition"), Listener.AddedCount, 1);

	// Without changing the source, a second commit must report nothing -- baseline was refreshed.
	Listener.AddedCount = 0;
	const bool bSecondReported = Fixture.Generator->TriggerCommitPostChangeSelectedValuesForTest();
	TestFalse(TEXT("Repeated commit without source change must return false"), bSecondReported);
	TestEqual(TEXT("Repeated commit must broadcast no Add"), Listener.AddedCount, 0);
	TestEqual(TEXT("Repeated commit must broadcast no Remove"), Listener.RemovedCount, 0);
	TestEqual(TEXT("Repeated commit must broadcast no Rename"), Listener.RenamedCount, 0);

	Listener.Unbind();
	Fixture.Teardown();
	return true;
}
