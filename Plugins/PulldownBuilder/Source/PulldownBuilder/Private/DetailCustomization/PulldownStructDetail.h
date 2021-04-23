// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DetailCustomizations.h"
#include "IPropertyTypeCustomization.h"

class SComboButton;
class FDetailWidgetRow;
class IDetailChildrenBuilder;
class IPropertyHandle;
struct FPulldownStructType;

/**
 * 
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

private:
	// Rebuilds the list of strings to display in the pull-down menu.
	void RebuildPulldown();

	// Search for the same name as the specified Name from the list of
	// character strings displayed in the pull-down menu.
	// If not found, returns nullptr.
	TSharedPtr<FString> FindDisplayNameByName(const FName& InName) const;
	
	// Called when the value of the combo box changes.
	void OnStateValueChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo);

	// Create a FUIAction from a copy-paste callback function.
	FUIAction CreateCopyAction();
	FUIAction CreatePasteAction();

	// Called when copying-pasting.
	void OnCopyAction();
	void OnPasteAction();
	
private:
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
