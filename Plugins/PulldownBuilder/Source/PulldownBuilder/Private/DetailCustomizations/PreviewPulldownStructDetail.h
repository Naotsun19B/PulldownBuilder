// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DetailCustomizations/PulldownStructDetail.h"

/**
 * Detail customization applied to structures that inherit from FPulldownStructBase.
 */
class PULLDOWNBUILDER_API FPreviewPulldownStructDetail : public FPulldownStructDetail
{
public:
	// Register-Unregister and instantiate this customization.
	static void Register();
	static void Unregister();
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

private:
	// FPulldownStructDetail interface.
	virtual TArray<TSharedPtr<FPulldownRow>> GenerateSelectableValues() override;
	// End of FPulldownStructDetail interface.
};
