// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DetailCustomizations.h"
#include "IPropertyTypeCustomization.h"
#include "PulldownBuilder/PulldownBuilderGlobals.h"

struct FPulldownStructType;
struct FPulldownRow;
class SPulldownSelectorComboButton;

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
	virtual TArray<TSharedPtr<FPulldownRow>> GenerateSelectableValues();
	
	// Behavior when multiple are selected. 
	virtual void OnMultipleSelected();

	// Returns whether the specified property is the property to be customized.
#if BEFORE_UE_4_24
	virtual bool IsCustomizationTarget(UProperty* InProperty) const;
#else
	virtual bool IsCustomizationTarget(FProperty* InProperty) const;
#endif

	// Set custom properties before and after FPulldownStructBase::SelectedValue.
	virtual void AddCustomRowBeforeSelectedValue(IDetailChildrenBuilder& StructBuilder) {}
	virtual void AddCustomRowAfterSelectedValue(IDetailChildrenBuilder& StructBuilder) {}

	// Generates a widget that displays a pull-down menu.
	TSharedRef<SWidget> GenerateSelectableValuesWidget();
	
	// Search for the same name as the specified name from the SelectableValues.
	// If not found, returns nullptr.
	TSharedPtr<FPulldownRow> FindSelectableValueByName(const FName& InName) const;
	
	// Called when the value of the SelectedValueWidget changes.
	void OnSelectedValueChanged(TSharedPtr<FPulldownRow> SelectedItem, ESelectInfo::Type SelectInfo);
	
	// Create a FUIAction from a copy-paste FPulldownStructBase::SelectedValue callback function.
	FUIAction CreateSelectedValueCopyAction();
	FUIAction CreateSelectedValuePasteAction();

	// Called when copying-pasting FPulldownStructBase::SelectedValue. 
	void OnSelectedValueCopyAction();
	void OnSelectedValuePasteAction();

protected:
	// The property handle of the structure that inherits the FPulldownStructBase
	// that is the target of this detail customization.
	TSharedPtr<IPropertyHandle> StructPropertyHandle;
	
	// A list of values that can be set in FPulldownStructBase::SelectedValue.
	TArray<TSharedPtr<FPulldownRow>> SelectableValues;

	// FPulldownStructBase::SelectedValue property handle.
	TSharedPtr<IPropertyHandle> SelectedValueHandle;

	// A widget that displays a pull-down menu based on the SelectableValues.
	TSharedPtr<SPulldownSelectorComboButton> SelectedValueWidget;

	// If this flag is true, inline display will not be performed regardless of the number of properties.
	bool bNeverInlineDisplay = false;
};
