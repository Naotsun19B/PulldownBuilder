// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DetailCustomization/PulldownStructDetailBase.h"

/**
 * Detail customization applied to structures that inherit from FPulldownStructBase.
 */
class PULLDOWNBUILDER_API FNativeLessPulldownStructDetail : public FPulldownStructDetailBase
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
	// FPulldownStructDetailBase interface.
	virtual void RebuildPulldown() override;
	// End of FPulldownStructDetailBase interface.

	// Search for the same name as the specified Name from the list of
	// character strings displayed in the pull-down menu.
	// If not found, returns nullptr.
	TSharedPtr<FString> FindPulldownContentsNameByName(const FName& InName) const;
	
	// Called when the value of the combo box changes.
	void OnSourceAssetChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo);
	
	// Create a FUIAction from a copy-paste callback function.
	FUIAction CreateCopyAction();
	FUIAction CreatePasteAction();

	// Called when copying-pasting.
	void OnCopyAction();
	void OnPasteAction();
	
private:
	// A list of strings to display in the pull-down menu.
	TArray<TSharedPtr<FString>> PulldownContentsNames;
	
	// FNativeLessPulldownStruct::PulldownContentsName property handle.
	TSharedPtr<IPropertyHandle> PulldownSourceHandle;

	// Pull-down menu widget.
	TSharedPtr<STextComboBox> SourceAssetPulldownWidget;
};
