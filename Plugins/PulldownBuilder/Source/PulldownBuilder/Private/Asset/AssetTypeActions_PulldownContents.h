// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

/**
 * 
 */
class FAssetTypeActions_PulldownContents : public FAssetTypeActions_Base
{
public:
	// FAssetTypeActions_Base interface.
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	virtual bool CanLocalize() const override;
	// End of FAssetTypeActions_Base interface.
};
