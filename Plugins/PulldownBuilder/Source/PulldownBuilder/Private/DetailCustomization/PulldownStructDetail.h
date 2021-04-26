// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DetailCustomizations.h"
#include "IPropertyTypeCustomization.h"

struct FPulldownStructType;

/**
 * Detail customization applied to structures that inherit from FPulldownStructBase.
 */
class PULLDOWNBUILDER_API FPulldownStructDetail : public IPropertyTypeCustomization
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

protected:
	// Rebuilds the list of strings to display in the pull-down menu.
	virtual void RebuildPulldown();

	// Refresh the widget in the pull-down menu.
	virtual void RefreshPulldownWidget();

	// Generates a list of strings to display in the pull-down menu.
	virtual TArray<TSharedPtr<FString>> GenerateSelectableValues();
	
	// Behavior when multiple are selected. 
	virtual void OnMutipleSelected();
	
	// Search for the same name as the specified name from the list of
	// character strings displayed in the pull-down menu.
	// If not found, returns nullptr.
	TSharedPtr<FString> FindSelectableValueByName(const FName& InName) const;

	// Generates a widget that displays a pull-down menu.
	TSharedRef<SWidget> GenerateSelectableValuesWidget();
	
	// Called when the value of the combo box changes.
	void OnSelectedValueChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo);
	
	// Create a FUIAction from a copy-paste FPulldownStructBase::SelectedValue callback function.
	FUIAction CreateSelectedValueCopyAction();
	FUIAction CreateSelectedValuePasteAction();

	// Called when copying-pasting FPulldownStructBase::SelectedValue. 
	void OnSelectedValueCopyAction();
	void OnSelectedValuePasteAction();

protected:
	// A list of strings to display in the pull-down menu.
	TArray<TSharedPtr<FString>> SelectableValues;

	// The property handle of the structure that inherits the FPulldownStructBase
	// that is the target of this detail customization.
	TSharedPtr<IPropertyHandle> StructPropertyHandle;

	// FPulldownStructBase::SelectedValue property handle.
	TSharedPtr<IPropertyHandle> SelectedValueHandle;

	// Pull-down menu widget.
	TSharedPtr<STextComboBox> SelectedValueWidget;
};
