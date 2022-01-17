// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "DataTableEditorUtils.h"
#include "DataTablePulldownListGenerator.generated.h"

// In UE 4.23, if a namespace is included in the inherited class of UCLASS,
// a build error will occur, so redefine it here.
using IDataTableListener = FDataTableEditorUtils::INotifyOnDataTableChanged;

/**
 * Generate a list to be displayed in the pull-down menu from the row name of the data table asset.
 * To set the text to display in a tooltip, you need to define a variable of
 * type FString named "PulldownTooltip" in the row structure.
 */
UCLASS()
class PULLDOWNBUILDER_API UDataTablePulldownListGenerator
	: public UPulldownListGeneratorBase
	, public IDataTableListener
{
	GENERATED_BODY()

public:
	// UPulldownListGeneratorBase interface.
	virtual TArray<TSharedPtr<FPulldownRow>> GetPulldownRows() const override;
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

protected:
	// The data table asset from which the list displayed in the pull-down menu is based.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pulldown")
	TSoftObjectPtr<UDataTable> SourceDataTable;

	// Cache of row list before change.
	UPROPERTY(Transient)
	TArray<FName> PreChangeRowList;
};
