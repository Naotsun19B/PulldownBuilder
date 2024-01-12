// Copyright 2021-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

struct FPulldownStructType;
struct FPulldownRow;
struct FUIAction;
class SWidget;
class UPulldownContents;

namespace PulldownBuilder
{
	class SPulldownSelectorComboButton;
	
	/**
	 * A detail customization class that applied to structures that inherit from FPulldownStructBase.
	 */
	class PULLDOWNBUILDER_API FPulldownStructDetail : public IPropertyTypeCustomization
	{
	public:
		// Registers-Unregisters and instantiate this customization.
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

		// Refreshes the widget in the pull-down menu.
		virtual void RefreshPulldownWidget();

		// Generates a list of strings to display in the pull-down menu.
		virtual TArray<TSharedPtr<FPulldownRow>> GenerateSelectableValues();
	
		// Called when multiple properties are selected.
		virtual void OnMultipleSelected();

		// Returns whether the specified property is the property to be customized.
#if UE_4_25_OR_LATER
		virtual bool IsCustomizationTarget(FProperty* InProperty) const;
#else
		virtual bool IsCustomizationTarget(UProperty* InProperty) const;
#endif

		// Sets custom properties before and after FPulldownStructBase::SelectedValue.
		virtual void AddCustomRowBeforeSelectedValue(IDetailChildrenBuilder& StructBuilder) {}
		virtual void AddCustomRowAfterSelectedValue(IDetailChildrenBuilder& StructBuilder) {}

		// Generates a widget that displays a pull-down menu.
		TSharedRef<SWidget> GenerateSelectableValuesWidget();
	
		// Finds for the same name as the specified name from the SelectableValues.
		// If not found, returns nullptr.
		TSharedPtr<FPulldownRow> FindSelectableValueByName(const FName& InName) const;

		// Returns the selected item.
		TSharedPtr<FPulldownRow> GetSelection() const;

		// Returns the PulldownContents asset associated with the currently edited pull-down struct.
		virtual UPulldownContents* GetRelatedPulldownContents() const;

		// Returns the individual panel size set in PulldownContents.
		float GetIndividualPanelHeight() const;
		float GetIndividualPanelWidth() const;
		
		// Called when the value of the SelectedValueWidget changes.
		void OnSelectedValueChanged(TSharedPtr<FPulldownRow> SelectedItem, ESelectInfo::Type SelectInfo);
	
		// Creates a FUIAction that works with the pull-down struct's context menu.
		FUIAction CreateSelectedValueCopyAction();
		FUIAction CreateSelectedValuePasteAction();
		FUIAction CreateBrowseSourceAssetAction();

		// Adds an action to browse source asset to the specified detail widget row.
		void AddBrowseSourceAssetAction(FDetailWidgetRow& DetailWidgetRow);
		
		// Called when a function is invoked from the pull-down struct's context menu.
		virtual void OnSelectedValueCopyAction();
		virtual void OnSelectedValuePasteAction();
		virtual void OnBrowseSourceAssetAction();

		// Returns whether the function can be called from the pull-down struct's context menu.
		virtual bool CanSelectedValueCopyAction() const;
		virtual bool CanSelectedValuePasteAction() const;
		virtual bool CanBrowseSourceAssetAction() const;

	protected:
		// The property handle of the struct that inherits the FPulldownStructBase that is the target of this detail customization.
		TSharedPtr<IPropertyHandle> StructPropertyHandle;
	
		// The list of values that can be set in FPulldownStructBase::SelectedValue.
		TArray<TSharedPtr<FPulldownRow>> SelectableValues;

		// The property handle of FPulldownStructBase::SelectedValue.
		TSharedPtr<IPropertyHandle> SelectedValueHandle;

		// The widget that displays a pull-down menu based on the SelectableValues.
		TSharedPtr<SPulldownSelectorComboButton> SelectedValueWidget;
	};
}
