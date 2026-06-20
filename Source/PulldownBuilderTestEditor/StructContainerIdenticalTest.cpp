// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "PulldownBuilder/Types/StructContainer.h"
#include "PulldownBuilderTest/TestPulldown.h"

/**
 * Regression tests for FStructContainer::Identical.
 *
 * Pre-fix bug (C-1): the inner condition was inverted -- two containers holding the SAME UScriptStruct
 * type were short-circuited to "not identical", and (because the inner CompareScriptStruct path was
 * unreachable) the function effectively reported identical values as non-identical.
 *
 * The cases below pin the fixed behaviour:
 *   1. nullptr argument           -> false
 *   2. same type, same value      -> true
 *   3. same type, different value -> false
 *   4. different types            -> false
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPulldownBuilderStructContainerIdenticalTest,
	"PulldownBuilder.Types.StructContainer.Identical",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FPulldownBuilderStructContainerIdenticalTest::RunTest(const FString& Parameters)
{
	UScriptStruct* PulldownStructType = FTestPulldown::StaticStruct();
	UScriptStruct* OtherPulldownStructType = FTestPulldown2::StaticStruct();
	TestNotNull(TEXT("FTestPulldown::StaticStruct() returns non-null"), PulldownStructType);
	TestNotNull(TEXT("FTestPulldown2::StaticStruct() returns non-null"), OtherPulldownStructType);

	// Case 1: nullptr -> false (defensive contract: Identical(nullptr, ...) must not crash and must return false).
	{
		FTestPulldown InstanceA(FName(TEXT("Foo")));
		const FStructContainer ContainerA(PulldownStructType, reinterpret_cast<const uint8*>(&InstanceA));

		TestFalse(TEXT("Identical(nullptr) must return false"), ContainerA.Identical(nullptr, PPF_None));
	}

	// Case 2: same type + same value -> true.
	{
		FTestPulldown InstanceA(FName(TEXT("Apple")));
		InstanceA.TestInt = 42;

		FTestPulldown InstanceB(FName(TEXT("Apple")));
		InstanceB.TestInt = 42;

		const FStructContainer ContainerA(PulldownStructType, reinterpret_cast<const uint8*>(&InstanceA));
		const FStructContainer ContainerB(PulldownStructType, reinterpret_cast<const uint8*>(&InstanceB));

		TestTrue(
			TEXT("Same type / same value containers must be Identical (regression for the C-1 inversion bug)"),
			ContainerA.Identical(&ContainerB, PPF_None)
		);
	}

	// Case 3: same type + different value -> false.
	{
		FTestPulldown InstanceA(FName(TEXT("Apple")));
		InstanceA.TestInt = 42;

		FTestPulldown InstanceB(FName(TEXT("Banana")));
		InstanceB.TestInt = 7;

		const FStructContainer ContainerA(PulldownStructType, reinterpret_cast<const uint8*>(&InstanceA));
		const FStructContainer ContainerB(PulldownStructType, reinterpret_cast<const uint8*>(&InstanceB));

		TestFalse(
			TEXT("Same type / different value containers must NOT be Identical"),
			ContainerA.Identical(&ContainerB, PPF_None)
		);
	}

	// Case 4: different struct types -> false.
	{
		FTestPulldown InstanceA(FName(TEXT("Apple")));
		FTestPulldown2 InstanceOther(FName(TEXT("Apple")));

		const FStructContainer ContainerA(PulldownStructType, reinterpret_cast<const uint8*>(&InstanceA));
		const FStructContainer ContainerOther(OtherPulldownStructType, reinterpret_cast<const uint8*>(&InstanceOther));

		TestFalse(
			TEXT("Different struct types must NOT be Identical, even with matching SelectedValue"),
			ContainerA.Identical(&ContainerOther, PPF_None)
		);
	}

	return true;
}
