// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Struct/PulldownStructType.h"
#include "Struct/PreviewPulldownStruct.h"
#include "PulldownContents.generated.h"

struct FPropertyChangedEvent;
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
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void BeginDestroy() override;
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
	// End of UObject interface.

	// Get information about the target structure.
	const FPulldownStructType& GetPulldownStructType() const { return PulldownStructType; }
	
	// Returns the list to display in the pull-down menu.
	virtual TArray<TSharedPtr<FString>> GetDisplayStrings() const;

protected:
	// Register the structure set for this asset in detail customization.
	virtual void RegisterDetailCustomization();
	
	// Unregister the structure set for this asset from detail customization.
	virtual void UnregisterDetailCustomization();
	
protected:	
	// Structure that displays a pull-down menu.
	// It must be a structure that inherits from FPulldownStructBase.
	UPROPERTY(EditDefaultsOnly)
	FPulldownStructType PulldownStructType;

	// A class that generates a list to display in the pull-down menu.
	// It can be freely expanded according to the application.
	UPROPERTY(EditDefaultsOnly, Instanced)
	UPulldownListGeneratorBase* PulldownListGenerator;

	// Shows a preview of the pull-down menu built from this PulldownContents.
	UPROPERTY(EditDefaultsOnly, Transient)
	FPreviewPulldownStruct Preview;
};
