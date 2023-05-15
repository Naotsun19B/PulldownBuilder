// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "DataTableEditorUtils.h"
#include "DataTablePulldownListGenerator.generated.h"

// In UE 4.23, if a namespace is included in the inherited class of UCLASS,
// a build error will occur, so redefine it here.
using IDataTableListener = FDataTableEditorUtils::INotifyOnDataTableChanged;

/**
 * A generator class that generates a list to be displayed in the pull-down menu from the row name of the data table asset.
 * To set the text to display in a tooltip, you need to define a variable of type FString or FName or FText named "PulldownTooltip" in
 * the row struct or specifies the FString or FName or FText property used in the tooltip,
 * such as TooltipProperty = "PropertyName" in the USTRUCT meta-specifier of the data table struct.
 */
UCLASS()
class PULLDOWNBUILDER_API UDataTablePulldownListGenerator
	: public UPulldownListGeneratorBase
	, public IDataTableListener
{
	GENERATED_BODY()

public:
	// The name of meta specifier for specifying properties for tooltips in USTRUCT.
	static const FString TooltipPropertyMeta;

	// The default name of the property that will be the data to be displayed as a tooltip in the pull-down menu.
	static const FString DefaultPulldownTooltipName;
	
public:
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

	// INotifyOnDataTableChanged interface.
	virtual void PreChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info) override;
	virtual void PostChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info) override;
	// End of INotifyOnDataTableChanged interface.

protected:
	// If it is looking for a property that satisfies the tooltip data condition,
	// it returns true and assigns the data to the TooltipString.
	// See the class description comments for property conditions.
	virtual bool FindTooltip(const UScriptStruct* RowStruct, uint8* RowData, FString& TooltipString) const;

	// Expansion points for implementation in blueprints.
	UFUNCTION(BlueprintImplementableEvent, Category = "Pulldown", meta = (Tooltip = "Called before a row name or data in the underlying data table is changed."))
	void PreSourceDataTableModify();
	UFUNCTION(BlueprintImplementableEvent, Category = "Pulldown", meta = (Tooltip = "Called after a row name or data in the underlying data table has been changed."))
	void PostSourceDataTableModify();
	
protected:
	// A data table asset from which the list displayed in the pull-down menu is based.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pulldown")
	mutable TSoftObjectPtr<UDataTable> SourceDataTable;

	// A cache of row list before change.
	UPROPERTY(Transient)
	TArray<FName> PreChangeRowNames;
};
