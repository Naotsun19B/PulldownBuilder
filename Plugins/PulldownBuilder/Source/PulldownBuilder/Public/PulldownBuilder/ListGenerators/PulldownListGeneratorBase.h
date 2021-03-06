// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PulldownBuilder/Types/PulldownRow.h"
#include "PulldownBuilder/Types/StructContainer.h"
#include "PulldownListGeneratorBase.generated.h"

/**
 * A generator class that generates a list to display in a pull-down menu.
 * You can filter the structures that can use this class by using the meta specifier in
 * the PulldownListGenerator class as follows:
 * UCLASS(meta = (FilterPulldownStructTypes = "<StructName>, <StructName>..."))
 */
UCLASS(Abstract, Blueprintable, EditInlineNew)
class PULLDOWNBUILDER_API UPulldownListGeneratorBase : public UObject
{
	GENERATED_BODY()

public:
	// The name of the meta-specifier that defines the types of structures that can use this pull-down list generator.
	static const FName FilterPulldownStructTypesName;
	
public:
	// Returns a list of data to display in the pull-down menu.
	// By default, it returns the value of "GetPulldownRowsFromBlueprint".
	virtual TArray<TSharedPtr<FPulldownRow>> GetPulldownRows(
		const TArray<UObject*>& OuterObjects,
		const FStructContainer& StructInstance
	) const;

	// Returns whether there is an underlying asset.
	// If this function returns true, the name of the original asset
	// will be displayed in the PulldownContents pop-up display.
	virtual bool HasSourceAsset() const;

	// Returns the name of the underlying asset only if "HasSourceAsset" returns true.
	virtual FString GetSourceAssetName() const;

	// Returns the types of structures that can use this pull-down list generator.
	virtual TArray<UScriptStruct*> GetFilterPulldownStructTypes() const;
	
protected:
	// Update all FPulldownStructBases that reference owner PulldownContents asset.
	virtual void UpdateDisplayStrings(const FName& PreChangeName, const FName& PostChangeName);
	
	// Expansion points for implementation in blueprints.
	UFUNCTION(BlueprintPure, BlueprintImplementableEvent, Category = "Pulldown", meta = (BlueprintProtected, DisplayName = "Get Pulldown Rows"))
    TArray<FPulldownRow> GetPulldownRowsFromBlueprint(
    	const TArray<UObject*>& OuterObjects,
    	const FStructContainer& StructInstance
    ) const;

	UFUNCTION(BlueprintPure, BlueprintImplementableEvent, Category = "Pulldown", meta = (BlueprintProtected, DisplayName = "Has Source Asset"))
	bool HasSourceAssetFromBlueprint() const;

	UFUNCTION(BlueprintPure, BlueprintImplementableEvent, Category = "Pulldown", meta = (BlueprintProtected, DisplayName = "Get Source Asset Name"))
	FString GetSourceAssetNameFromBlueprint() const;

	UFUNCTION(BlueprintPure, BlueprintImplementableEvent, Category = "Pulldown", meta = (BlueprintProtected, DisplayName = "Get Filter Pulldown Struct Types"))
	TArray<FName> GetFilterPulldownStructTypesFromBlueprint() const;
};
