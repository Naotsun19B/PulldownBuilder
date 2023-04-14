﻿// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "StringTablePulldownListGenerator.generated.h"

/**
 * Generate a list to be displayed in the pull-down menu from the row name of the string table asset.
 */
UCLASS()
class PULLDOWNBUILDER_API UStringTablePulldownListGenerator
	: public UPulldownListGeneratorBase
	, public FTickableEditorObject
{
	GENERATED_BODY()

public:
	// Constructor.
	UStringTablePulldownListGenerator();
	
	// UObject interface.
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface.
	
	// UPulldownListGeneratorBase interface.
	virtual TArray<TSharedPtr<FPulldownRow>> GetPulldownRows(
		const TArray<UObject*>& OuterObjects,
		const FStructContainer& StructInstance
	) const override;
	virtual bool HasSourceAsset() const override;
	virtual FString GetSourceAssetName() const override;
	// End of UPulldownListGeneratorBase interface.

	// FTickableObjectBase interface.
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	// End of FTickableObjectBase interface.
	
protected:
	// Cache a pull-down list of current keys of string table.
	void CacheStringTableKeys(TArray<FName>& StringTableKeys) const;
	
protected:
	// The string table asset from which the list displayed in the pull-down menu is based.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pulldown")
	mutable TSoftObjectPtr<UStringTable> SourceStringTable;

	// Whether PostInitProperties was called.
	UPROPERTY(Transient)
	bool bInitialized;
	
	// Cache of row list before change.
	UPROPERTY(Transient)
	TArray<FName> PreChangeRowNames;
};
