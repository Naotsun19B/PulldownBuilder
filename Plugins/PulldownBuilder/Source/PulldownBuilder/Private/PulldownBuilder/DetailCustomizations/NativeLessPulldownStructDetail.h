// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilder/DetailCustomizations/PulldownStructDetail.h"

struct FPulldownRow;

namespace PulldownBuilder
{
	class SPulldownSelectorComboButton;
	class ICustomPropertyTypeLayoutRegistry;
	
	/**
	 * A detail customization class that applied to structures that inherit from FPulldownStructBase.
	 */
	class PULLDOWNBUILDER_API FNativeLessPulldownStructDetail : public FPulldownStructDetail
	{
	public:
		// Registers-Unregisters and instantiate this customization.
		static void Register();
		static void Unregister();

		// IPropertyTypeCustomization interface.
		virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
		// End of IPropertyTypeCustomization interface.

	private:
		// FPulldownStructDetail interface.
		virtual bool ShouldInlineDisplay() const override;
		virtual void RefreshPulldownWidget() override;
		virtual FPulldownRows GenerateSelectableValues() override;
		virtual void OnMultipleSelected() override;
		virtual UPulldownContents* GetRelatedPulldownContents() const override;
		virtual void AddCustomRowBeforeSelectedValue(IDetailChildrenBuilder& StructBuilder) override;
		virtual void OnBrowseSourceAssetAction() override;
		virtual bool CanBrowseSourceAssetAction() const override;
		// End of FPulldownStructDetail interface.

		// Finds for the same name as the specified name from the PulldownContentsNames.
		TSharedPtr<FPulldownRow> FindPulldownContentsNameByName(const FName& InName) const;

		// Returns the selected pull-down source.
		TSharedPtr<FPulldownRow> GetPulldownSourceSelection() const;
		
		// Called when the value of the PulldownSourceWidget changes.
		void OnPulldownSourceChanged(TSharedPtr<FPulldownRow> SelectedItem, ESelectInfo::Type SelectInfo);

		// Creates a FUIAction from a copy-paste FNativeLessPulldownStruct::PulldownSource callback function.
		FUIAction CreatePulldownSourceCopyAction();
		FUIAction CreatePulldownSourcePasteAction();

		// Called when copying-pasting FNativeLessPulldownStruct::PulldownSource. 
		void OnPulldownSourceCopyAction();
		void OnPulldownSourcePasteAction();
	
	private:
		// The list of values that can be set in FNativeLessPulldownStruct::PulldownSource.
		FPulldownRows PulldownContentsNames;
	
		// The property handle of FNativeLessPulldownStruct::PulldownContentsName.
		TSharedPtr<IPropertyHandle> PulldownSourceHandle;

		// The widget that displays a pull-down menu based on the PulldownContentsNames.
		TSharedPtr<SPulldownSelectorComboButton> PulldownSourceWidget;

		// The instance of the registry that registers this custom detail panel in the property editor.
		static TUniquePtr<ICustomPropertyTypeLayoutRegistry> CustomPropertyTypeLayoutRegistry;
	};
}
