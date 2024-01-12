// Copyright 2021-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PulldownBuilder/Types/PulldownRow.h"
#include "PulldownBuilder/Types/StructContainer.h"
#include "PulldownListGeneratorBase.generated.h"

/**
 * A generator class that generates a list to display in a pull-down menu.
 * You can filter the structures that can use this class by using the meta specifier in the PulldownListGenerator class as follows:
 * 
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
	// If this function returns true, the name of the original asset will be displayed in the PulldownContents pop-up display.
	virtual bool HasSourceAsset() const;

	// Returns the name of the underlying asset only if "HasSourceAsset" returns true.
	virtual FString GetSourceAssetName() const;

	// Returns the types of structures that can use this pull-down list generator.
	virtual TArray<UScriptStruct*> GetFilterPulldownStructTypes() const;
	
protected:
	// Notifies a value added to the pull-down menu.
	virtual void NotifyPulldownRowAdded(const FName& AddedChangeName);

	// Notifies a value removed from the pull-down menu.
	virtual void NotifyPulldownRowRemoved(const FName& RemovedChangeName);
	
	// Notifies a value contained in the pull-down menu has been renamed.
	virtual void NotifyPulldownRowRenamed(const FName& PreChangeName, const FName& PostChangeName);

	// Notifies the underlying data for the pull-down menu has changed.
	virtual void NotifyPulldownContentsSourceChanged();
	
	// Notifies of changes to pull-down menus and returns whether or not they were actually notified.
	bool NotifyPulldownRowChanged(const TArray<FName>& PreChangeRowNames, const TArray<FName>& PostChangeRowNames);
	
	// Expansion points for implementation in blueprints.
	UFUNCTION(BlueprintPure, BlueprintImplementableEvent, Category = "Pulldown", meta = (BlueprintProtected, DisplayName = "Get Pulldown Rows", Tooltip = "Returns a list of data to display in the pull-down menu."))
    TArray<FPulldownRow> GetPulldownRowsFromBlueprint(
    	const TArray<UObject*>& OuterObjects,
    	const FStructContainer& StructInstance
    ) const;

	UFUNCTION(BlueprintPure, BlueprintImplementableEvent, Category = "Pulldown", meta = (BlueprintProtected, DisplayName = "Has Source Asset", Tooltip = "Returns whether there is an underlying asset.\nIf this function returns true, the name of the original asset will be displayed in the PulldownContents pop-up display."))
	bool HasSourceAssetFromBlueprint() const;

	UFUNCTION(BlueprintPure, BlueprintImplementableEvent, Category = "Pulldown", meta = (BlueprintProtected, DisplayName = "Get Source Asset Name", Tooltip = "Returns the name of the underlying asset only if \"HasSourceAsset\" returns true."))
	FString GetSourceAssetNameFromBlueprint() const;

	UFUNCTION(BlueprintPure, BlueprintImplementableEvent, Category = "Pulldown", meta = (BlueprintProtected, DisplayName = "Get Filter Pulldown Struct Types", Tooltip = "Returns the types of structures that can use this pull-down list generator."))
	TArray<FName> GetFilterPulldownStructTypesFromBlueprint() const;
};
