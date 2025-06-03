// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PulldownBuilderTest/TestPulldown.h"
#include "PulldownBuilderTestEditorSettings.generated.h"

UCLASS(Config = Editor, DefaultConfig)
class PULLDOWNBUILDERTESTEDITOR_API UPulldownBuilderTestEditorSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category = "Test")
	FTestPulldown TestPulldown;

public:
	static void Register();
	static void Unregister();
};
