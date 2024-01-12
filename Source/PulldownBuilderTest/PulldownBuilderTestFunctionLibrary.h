// Copyright 2021-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PulldownBuilderTestFunctionLibrary.generated.h"

UCLASS()
class PULLDOWNBUILDERTEST_API UPulldownBuilderTestFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Pulldown Builder Test", meta = (AutoCreateRefTerm = "Message"))
	static void Log(const FText& Message);
};
