// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

/**
 * A class that defines the operations that can be performed on an PulldownContents and
 * information about the PulldownContents.
 */
class PULLDOWNBUILDER_API FAssetTypeActions_PulldownContents : public FAssetTypeActions_Base
{
public:
	// Register-Unregister and instance getter this class.
	static void Register();
	static void Unregister();
	static FAssetTypeActions_PulldownContents& Get();
	
	// FAssetTypeActions_Base interface.
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	virtual bool CanLocalize() const override;
	// End of FAssetTypeActions_Base interface.

private:
	// An instance of this style class.
	static TSharedPtr<FAssetTypeActions_PulldownContents> Instance;
};
