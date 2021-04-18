// Copyright 2021 Naotsun. All Rights Reserved.

#include "Common/PulldownBuilderUtils.h"
#include "PulldownStructBase.h"

bool FPulldownBuilderUtils::IsPulldownStruct(const UScriptStruct* InStruct)
{
	bool bValidStruct = false;
	bool bBasedOnPulldownStructBase = false;
	if (UScriptStruct* PulldownStruct = FPulldownStructBase::StaticStruct())
	{
		bValidStruct = (InStruct->GetOutermost() != GetTransientPackage());
		bBasedOnPulldownStructBase = PulldownStruct && InStruct->IsChildOf(PulldownStruct) && (InStruct != PulldownStruct);
	}

	return (bValidStruct && bBasedOnPulldownStructBase);
}
