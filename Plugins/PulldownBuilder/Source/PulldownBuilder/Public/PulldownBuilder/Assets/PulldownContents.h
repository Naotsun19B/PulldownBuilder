﻿// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/PackageReload.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "PulldownBuilder/Types/PulldownStructType.h"
#include "PulldownBuilder/Types/PreviewPulldownStruct.h"
#include "PulldownContents.generated.h"

struct FPulldownRow;
struct FStructContainer;
class UPulldownListGeneratorBase;

/**
 * An asset class that defines a struct that displays a pull-down menu and builds the list that is the basis of the pull-down menu.
 */
UCLASS()
class PULLDOWNBUILDER_API UPulldownContents : public UObject
{
	GENERATED_BODY()

public:
	// Define tags for the information that is displayed when you hover over an asset in the Content Browser.
	static const FName RegisteredStructTypeTag;
	static const FName GeneratorClassTag;
	static const FName SourceAssetTag;
	
public:
	// UObject interface.
	virtual bool IsEditorOnly() const override;
	virtual void PostLoad() override;
#if UE_4_25_OR_LATER
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
#else
	virtual void PreEditChange(UProperty* PropertyAboutToChange) override;
#endif
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void BeginDestroy() override;
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
	// End of UObject interface.

	// Gets information about the target struct.
	const FPulldownStructType& GetPulldownStructType() const;
	
	// Returns the data list to display in the pull-down menu.
	virtual TArray<TSharedPtr<FPulldownRow>> GetPulldownRows(
		const TArray<UObject*>& OuterObjects,
		const FStructContainer& StructInstance
	) const;

	// Returns the name of the class set in the PulldownListGenerator.
	virtual FString GetPulldownListGeneratorClassName() const;
	
	// Returns the text of the tooltip to display with NativeLessPulldownStruct.
	virtual FString GetTooltip() const;
	
protected:
	// Registers the struct set for this asset in detail customization.
	virtual void RegisterDetailCustomization();
	
	// Unregisters the struct set for this asset from detail customization.
	virtual void UnregisterDetailCustomization();

	// Called when the properties of the PulldownListGenerator need to be changed.
	virtual void ModifyPulldownListGenerator();

	// Called when the asset editor is opened.
	void HandleOnAssetEditorOpened(UObject* OpenedAsset);
	
	// Called when any asset in any package is reloaded.
	void HandleOnPackageReloaded(EPackageReloadPhase ReloadPhase, FPackageReloadedEvent* ReloadedEvent);
	
protected:	
	// The struct type that displays a pull-down menu.
	// It must be a struct that inherits from FPulldownStructBase.
	UPROPERTY(EditDefaultsOnly, Category = "Pulldown")
	FPulldownStructType PulldownStructType;

	// The class that generates a list to display in the pull-down menu.
	// It can be freely expanded according to the application.
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Pulldown")
	UPulldownListGeneratorBase* PulldownListGenerator;

	// Shows a preview of the pull-down menu built from this PulldownContents.
	UPROPERTY(EditDefaultsOnly, Transient, Category = "Pulldown")
	FPreviewPulldownStruct Preview;

	// The cache to restore when the PulldownListGenerator cannot be changed.
	UPROPERTY(Transient)
	UPulldownListGeneratorBase* PreChangePulldownListGenerator;
};
