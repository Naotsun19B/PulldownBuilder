// Copyright 2021 Naotsun. All Rights Reserved.

#include "TestPulldown.h"

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
