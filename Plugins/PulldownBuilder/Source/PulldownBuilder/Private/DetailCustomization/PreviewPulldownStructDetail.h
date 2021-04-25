// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DetailCustomization/PulldownStructDetailBase.h"

/**
 * Detail customization applied to structures that inherit from FPulldownStructBase.
 */
class PULLDOWNBUILDER_API FPreviewPulldownStructDetail : public FPulldownStructDetailBase
{
public:
	// Register-Unregister and instantiate this customization.
	static void Register();
	static void Unregister();
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	// IPropertyTypeCustomization interface.
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	// End of IPropertyTypeCustomization interface.

private:
	// Rebuilds the list of strings to display in the pull-down menu.
	virtual void RebuildPulldown() override;
};
