// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ListGenerators/PulldownListGeneratorBase.h"
#include "DataTableEditorUtils.h"
#include "DataTablePulldownListGenerator.generated.h"

// In UE 4.23, if a namespace is included in the inherited class of UCLASS,
// a build error will occur, so redefine it here.
using IDataTableListener = FDataTableEditorUtils::INotifyOnDataTableChanged;

/**
 * Generate a list to be displayed in the pull-down menu from the row name of the data table asset.
 */
UCLASS()
class UDataTablePulldownListGenerator
	: public UPulldownListGeneratorBase
	, public IDataTableListener
{
	GENERATED_BODY()

public:
	// UPulldownListGeneratorBase interface.
	virtual TArray<TSharedPtr<FString>> GetDisplayStrings() const override;
	// End of UPulldownListGeneratorBase interface.

	// INotifyOnDataTableChanged interface.
	virtual void PreChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info) override;
	virtual void PostChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info) override;
	// End of INotifyOnDataTableChanged interface.

protected:
	// The data table asset from which the list displayed in the pull-down menu is based.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pulldown")
	TSoftObjectPtr<UDataTable> SourceDataTable;

	// Cache of row list before change.
	UPROPERTY(Transient)
	TArray<FName> PreChangeRowList;
};
