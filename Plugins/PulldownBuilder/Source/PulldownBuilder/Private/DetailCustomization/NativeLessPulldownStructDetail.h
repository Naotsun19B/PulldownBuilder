// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DetailCustomization/PulldownStructDetail.h"

class SSearchableTextComboBox;

/**
 * Detail customization applied to structures that inherit from FPulldownStructBase.
 */
class PULLDOWNBUILDER_API FNativeLessPulldownStructDetail : public FPulldownStructDetail
{
public:
	// Register-Unregister and instantiate this customization.
	static void Register();
	static void Unregister();
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	// IPropertyTypeCustomization interface.
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	// End of IPropertyTypeCustomization interface.

private:
	// FPulldownStructDetail interface.
	virtual void RefreshPulldownWidget() override;
	virtual TArray<TSharedPtr<FString>> GenerateSelectableValues() override;
	virtual void OnMultipleSelected() override;
#if BEFORE_UE_4_24
	virtual bool IsCustomizationTarget(UProperty* InProperty) const;
#else
	virtual bool IsCustomizationTarget(FProperty* InProperty) const;
#endif
	virtual void AddCustomRowBeforeSelectedValue(IDetailChildrenBuilder& StructBuilder) override;
	// End of FPulldownStructDetail interface.

	// Search for the same name as the specified name from the PulldownContentsNames.
	// If not found, returns nullptr.
	TSharedPtr<FString> FindPulldownContentsNameByName(const FName& InName) const;
	
	// Called when the value of the PulldownSourceWidget changes.
	void OnPulldownSourceChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo);
	
	// Create a FUIAction from a copy-paste FNativeLessPulldownStruct::PulldownSource callback function.
	FUIAction CreatePulldownSourceCopyAction();
	FUIAction CreatePulldownSourcePasteAction();

	// Called when copying-pasting FNativeLessPulldownStruct::PulldownSource. 
	void OnPulldownSourceCopyAction();
	void OnPulldownSourcePasteAction();
	
private:
	// A list of values that can be set in FNativeLessPulldownStruct::PulldownSource.
	TArray<TSharedPtr<FString>> PulldownContentsNames;
	
	// FNativeLessPulldownStruct::PulldownContentsName property handle.
	TSharedPtr<IPropertyHandle> PulldownSourceHandle;

	// A widget that displays a pull-down menu based on the PulldownContentsNames.
	TSharedPtr<SSearchableTextComboBox> PulldownSourceWidget;
};
