﻿// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"

class SWidget;
class SComboButton;
class FDetailWidgetRow;
class IDetailChildrenBuilder;
class IPropertyHandle;

namespace PulldownBuilder
{
	/**
	 * Detail Customizetion that allows you to specify a struct that inherits from 
	 * FPulldownStructBase using the struct picker.
	 */
	class PULLDOWNBUILDER_API FPulldownStructTypeDetail : public IPropertyTypeCustomization
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
		// Called when a struct is selected from the list.
		void OnPickedStruct(const UScriptStruct* SelectedStruct);

		// Returns the text displayed on the combo button.
		FText OnGetComboTextValue() const;

		// Create the same struct picker that you see when you create the data table asset.
		TSharedRef<SWidget> GenerateStructPicker();

	private:
		// Handle for accessing FPulldownStructType::SelectedStruct.
		TSharedPtr<IPropertyHandle> SelectedStructHandle;

		// ComboButton to launch the struct picker when you click on a property.
		TSharedPtr<SComboButton> StructPickerAnchor;

		// A cache of type names for properties that utilize this details panel.
		static FString CachedPropertyTypeName;
	};
}
