// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "PulldownBuilder/Types/PulldownStructType.h"
#include "PulldownBuilder/Types/PreviewPulldownStruct.h"
#include "PulldownContents.generated.h"

struct FPulldownRow;
class UPulldownListGeneratorBase;

/**
 * A class that defines a structure that displays a pull-down menu and 
 * builds the list that is the basis of the pull-down menu.
 */
UCLASS()
class PULLDOWNBUILDER_API UPulldownContents : public UObject
{
	GENERATED_BODY()
	
public:
	// UObject interface.
	virtual bool IsEditorOnly() const override { return true; }
	virtual void PostLoad() override;
#if BEFORE_UE_4_24
	virtual void PreEditChange(UProperty* PropertyAboutToChange) override;
#else
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
#endif
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void BeginDestroy() override;
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
	// End of UObject interface.

	// Get information about the target structure.
	const FPulldownStructType& GetPulldownStructType() const { return PulldownStructType; }
	
	// Returns the data list to display in the pull-down menu.
	virtual TArray<TSharedPtr<FPulldownRow>> GetPulldownRows() const;

	// Returns the name of the class set in the PulldownListGenerator.
	virtual FString GetPulldownListGeneratorClassName() const;
	
	// Returns the text of the tooltip to display with NativeLessPulldownStruct.
	virtual FString GetTooltip() const;
	
protected:
	// Register the structure set for this asset in detail customization.
	virtual void RegisterDetailCustomization();
	
	// Unregister the structure set for this asset from detail customization.
	virtual void UnregisterDetailCustomization();
	
protected:	
	// Structure that displays a pull-down menu.
	// It must be a structure that inherits from FPulldownStructBase.
	UPROPERTY(EditDefaultsOnly, Category = "Pulldown")
	FPulldownStructType PulldownStructType;

	// A class that generates a list to display in the pull-down menu.
	// It can be freely expanded according to the application.
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Pulldown")
	UPulldownListGeneratorBase* PulldownListGenerator;

	// Shows a preview of the pull-down menu built from this PulldownContents.
	UPROPERTY(EditDefaultsOnly, Transient, Category = "Pulldown")
	FPreviewPulldownStruct Preview;
};
