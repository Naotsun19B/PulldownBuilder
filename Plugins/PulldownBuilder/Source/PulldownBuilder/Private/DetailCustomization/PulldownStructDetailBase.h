// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DetailCustomizations.h"
#include "IPropertyTypeCustomization.h"

class STextComboBox;
class FDetailWidgetRow;
class IDetailChildrenBuilder;
class IPropertyHandle;

/**
 * Base class of detail customization that displays a pull-down menu for a structure that inherits FPulldownStructBase.
 */
class PULLDOWNBUILDER_API FPulldownStructDetailBase : public IPropertyTypeCustomization
{
public:
	// IPropertyTypeCustomization interface.
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	// End of IPropertyTypeCustomization interface.

protected:
	// Rebuilds the list of strings to display in the pull-down menu.
	virtual void RebuildPulldown();

	// Search for the same name as the specified Name from the list of
	// character strings displayed in the pull-down menu.
	// If not found, returns nullptr.
	TSharedPtr<FString> FindDisplayStringByName(const FName& InName) const;
	
	// Called when the value of the combo box changes.
	void OnStateValueChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo);
	
protected:
	// A list of strings to display in the pull-down menu.
	TArray<TSharedPtr<FString>> DisplayStrings;

	// The property handle of the structure that inherits the FPulldownStructBase
	// that is the target of this detail customization.
	TSharedPtr<IPropertyHandle> StructPropertyHandle;

	// FPulldownStructBase::SelectedValue property handle.
	TSharedPtr<IPropertyHandle> SelectedValueHandle;

	// Pull-down menu widget.
	TSharedPtr<STextComboBox> PulldownWidget;
};
