// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilder/DetailCustomizations/PulldownStructDetail.h"

struct FPulldownRow;

namespace PulldownBuilder
{
	class SPulldownSelectorComboButton;
	
	/**
	 * A detail customization class that applied to structures that inherit from FPulldownStructBase.
	 */
	class PULLDOWNBUILDER_API FNativeLessPulldownStructDetail : public FPulldownStructDetail
	{
	public:
		// Registers-Unregisters and instantiate this customization.
		static void Register();
		static void Unregister();
		static TSharedRef<IPropertyTypeCustomization> MakeInstance();

		// IPropertyTypeCustomization interface.
		virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
		// End of IPropertyTypeCustomization interface.

	private:
		// FPulldownStructDetail interface.
		virtual void RefreshPulldownWidget() override;
		virtual TArray<TSharedPtr<FPulldownRow>> GenerateSelectableValues() override;
		virtual void OnMultipleSelected() override;
#if UE_4_25_OR_LATER
		virtual bool IsCustomizationTarget(FProperty* InProperty) const override;
#else
		virtual bool IsCustomizationTarget(UProperty* InProperty) const override;
#endif
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
		// A list of values that can be set in FNativeLessPulldownStruct::PulldownSource.
		TArray<TSharedPtr<FPulldownRow>> PulldownContentsNames;
	
		// A property handle of FNativeLessPulldownStruct::PulldownContentsName.
		TSharedPtr<IPropertyHandle> PulldownSourceHandle;

		// A widget that displays a pull-down menu based on the PulldownContentsNames.
		TSharedPtr<SPulldownSelectorComboButton> PulldownSourceWidget;

		// A cache of type names for properties that utilize this details panel.
		static FString CachedPropertyTypeName;
	};
}
