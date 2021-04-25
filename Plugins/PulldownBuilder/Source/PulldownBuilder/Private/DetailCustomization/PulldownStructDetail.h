// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DetailCustomization/PulldownStructDetailBase.h"

struct FPulldownStructType;

/**
 * Detail customization applied to structures that inherit from FPulldownStructBase.
 */
class PULLDOWNBUILDER_API FPulldownStructDetail : public FPulldownStructDetailBase
{
public:
	// Register-Unregister and instantiate this customization.
	static void Register(const FPulldownStructType& StructType);
	static void Unregister(const FPulldownStructType& StructType);
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	// IPropertyTypeCustomization interface.
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	// End of IPropertyTypeCustomization interface.

private:
	// FPulldownStructDetailBase interface.
	virtual void RebuildPulldown() override;
	// End of FPulldownStructDetailBase interface.

	// Create a FUIAction from a copy-paste callback function.
	FUIAction CreateCopyAction();
	FUIAction CreatePasteAction();

	// Called when copying-pasting.
	void OnCopyAction();
	void OnPasteAction();
};
