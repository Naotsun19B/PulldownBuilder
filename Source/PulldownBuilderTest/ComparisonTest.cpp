// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilderTest/TestPulldown.h"

namespace ComparisonTest
{
	void TestFunction()
	{
		FTestPulldown TestA;
		FTestPulldown TestB;
		FTestPulldown2 Test2;

		// Only the same type can be compared.

		// OK.
		const bool bSameType = (TestA == TestB);
		// Error.
		//const bool bDifferentType = (TestA != Test2);
	}
}
