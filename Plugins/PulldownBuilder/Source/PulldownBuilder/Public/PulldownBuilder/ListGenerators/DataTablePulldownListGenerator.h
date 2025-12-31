// Copyright 2021-2026 Naotsun. All Rights Reserved.

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
 *
 * To specify each of the following elements, you must either define property of a given type,
 * or specify property of a given type using the meta specifier specified in the USTRUCT of the row struct.
 *
 * - TooltipText
 *		- Types:			FString  FName  FText
 *		- Property Name:	PulldownTooltip
 *		- Meta Specifier:	TooltipProperty = "PropertyName"
 *
 * - DisplayTextColor
 *		- Types:			FColor  FLinearColor  FSlateColor
 *		- Property Name:	PulldownTextColor
 *		- Meta Specifier:	TextColorProperty = "PropertyName"
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

	// The name of meta specifier for specifying properties for text color in USTRUCT.
	static const FString TextColorPropertyMeta;

	// The default name of the property that will be the data to be displayed as a text colo in the pull-down menu.
	static const FString DefaultPulldownTextColorName;
	
public:
	// UObject interface.
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface.
	
	// UPulldownListGeneratorBase interface.
	virtual FPulldownRows GetPulldownRows(
		const TArray<UObject*>& OuterObjects,
		const FStructContainer& StructInstance
	) const override;
	virtual bool HasSourceAsset_Implementation() const override;
	virtual FString GetSourceAssetName_Implementation() const override;
	// End of UPulldownListGeneratorBase interface.

	// INotifyOnDataTableChanged interface.
	virtual void PreChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info) override;
	virtual void PostChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info) override;
	// End of INotifyOnDataTableChanged interface.

protected:
	// If it is looking for a property that satisfies the tooltip data condition, it returns true and assigns the data to the TooltipText.
	// See the class description comments for property conditions.
	virtual bool FindTooltip(const UScriptStruct* RowStruct, uint8* RowData, FText& TooltipText) const;

	// If it is looking for a property that satisfies the text color data condition, it returns true and assigns the data to the TextColor.
	// See the class description comments for property conditions.
	virtual bool FindTextColor(const UScriptStruct* RowStruct, uint8* RowData, FLinearColor& TextColor) const;

	// Expansion points for implementation in blueprints.
	UFUNCTION(BlueprintImplementableEvent, Category = "Pulldown", meta = (Tooltip = "Called before a row name or data in the underlying data table is changed."))
	void PreSourceDataTableModify();
	UFUNCTION(BlueprintImplementableEvent, Category = "Pulldown", meta = (Tooltip = "Called after a row name or data in the underlying data table has been changed."))
	void PostSourceDataTableModify();

	// UPulldownListGeneratorBase interface.
	virtual TArray<FName> GetDefaultValueOptions_Implementation() const override;
	// End of UPulldownListGeneratorBase interface.
	
protected:
	// The data table asset from which the list displayed in the pull-down menu is based.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pulldown")
	mutable TSoftObjectPtr<UDataTable> SourceDataTable;

	// The cache of row list before change.
	UPROPERTY(Transient)
	TArray<FName> PreChangeRowNames;
};
