// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownStruct/NativeLessPulldownStruct.h"

FNativeLessPulldownStruct::FNativeLessPulldownStruct()
	: FPulldownStructBase(NAME_None)
	, PulldownSource(NAME_None)
{
}

FNativeLessPulldownStruct::FNativeLessPulldownStruct(const FName& InPulldownSource, const FName& InSelectedValue)
	: FPulldownStructBase(InPulldownSource)
	, PulldownSource(InSelectedValue)
{
}
