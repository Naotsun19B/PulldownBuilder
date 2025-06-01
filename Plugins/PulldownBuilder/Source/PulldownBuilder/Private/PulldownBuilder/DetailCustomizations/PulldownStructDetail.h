// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "PropertyHandle.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "PulldownBuilder/Types/PulldownRows.h"

struct FPulldownStructType;
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
		// Initializes pull-down menu and widget when the details panel is constructed.
		virtual void InitializePulldown();
		
		// Rebuilds the list of strings to display in the pull-down menu.
		virtual void RebuildPulldown();

		// Refreshes the widget in the pull-down menu.
		virtual void RefreshPulldownWidget();

		// Returns whether the pull-down struct being edited should be displayed inline.
		virtual bool ShouldInlineDisplay() const;

		// Generates a list of strings to display in the pull-down menu.
		virtual FPulldownRows GenerateSelectableValues();
		
		// Called when multiple properties are selected.
		virtual void OnMultipleSelected();

		// Sets custom properties before and after FPulldownStructBase::SelectedValue.
		virtual void AddCustomRowBeforeSelectedValue(IDetailChildrenBuilder& StructBuilder) {}
		virtual void AddCustomRowAfterSelectedValue(IDetailChildrenBuilder& StructBuilder) {}

		// Returns the PulldownContents asset associated with the currently edited pull-down struct.
		virtual UPulldownContents* GetRelatedPulldownContents() const;

		// Returns type information of the pull-down struct currently being edited.
		const UScriptStruct* GetRelatedPulldownStructType() const;
		
		// Generates a widget that displays a pull-down menu.
		TSharedRef<SWidget> GenerateSelectableValuesWidget();

		// Safely changes the value of a property handle.
		// NOTE: There are parts of the MovieScene module code that don't check for nullptr.
		//       This causes a crash when changing properties in the sequencer editor, so until this is fixed,
		//       for MovieSceneSignedObject (ex: sections or tracks), set to new value at the next frame.
		template<typename TPropertyValue>
		static void SetPropertyValueSafe(const TSharedRef<IPropertyHandle>& TargetPropertyHandle, const TPropertyValue& NewValue)
		{
			SetPropertyValueSafeImpl(
				TargetPropertyHandle,
				[NewValue](const TSharedRef<IPropertyHandle>& PropertyHandle)
				{
					PropertyHandle->SetValue(NewValue);
				}
			);
		}
		static void SetPropertyValueSafeImpl(
			const TSharedRef<IPropertyHandle>& TargetPropertyHandle,
			const TFunction<void(const TSharedRef<IPropertyHandle>& PropertyHandle)>& Predicate
		);
	
		// Finds for the same name as the specified name from the SelectableValues.
		// If not found, returns nullptr.
		TSharedPtr<FPulldownRow> FindSelectableValueByName(const FName& InName) const;

		// Returns the selected item.
		TSharedPtr<FPulldownRow> GetSelection() const;

		// Returns the individual panel size set in PulldownContents.
		float GetIndividualPanelHeight() const;
		float GetIndividualPanelWidth() const;
		
		// Called when the value of the SelectedValueWidget changes.
		void OnSelectedValueChanged(TSharedPtr<FPulldownRow> SelectedItem, ESelectInfo::Type SelectInfo);

		// Updates the value of a pull-down struct's searchable object.
		void UpdateSearchableObject();

		// Applies default values to the pull-down structure being edited.
		void ApplyDefaultValue();

		// Returns whether the currently edited pull-down struct has been changed at least once.
		bool IsEdited() const;

		// Initializes the details panel widget.
		virtual void ConstructDetailWidgetRow(FDetailWidgetRow& DetailWidgetRow);
		
		// Creates a FUIAction that works with the pull-down struct's context menu.
		FUIAction CreateSelectedValueCopyAction();
		FUIAction CreateSelectedValuePasteAction();
		
		// Called when a function is invoked from the pull-down struct's context menu.
		virtual void OnSelectedValueCopyAction();
		virtual void OnSelectedValuePasteAction();
		virtual void OnResetToDefaultAction(TSharedPtr<IPropertyHandle> PropertyHandle);
		virtual void OnBrowseSourceAssetAction();

		// Returns whether the function can be called from the pull-down struct's context menu.
		virtual bool CanSelectedValueCopyAction() const;
		virtual bool CanSelectedValuePasteAction() const;
		virtual bool CanResetToDefaultAction(TSharedPtr<IPropertyHandle> PropertyHandle) const;
		virtual bool CanBrowseSourceAssetAction() const;

	protected:
		// The property handle of the struct that inherits the FPulldownStructBase that is the target of this detail customization.
		TSharedPtr<IPropertyHandle> StructPropertyHandle;
	
		// The list of values that can be set in FPulldownStructBase::SelectedValue.
		FPulldownRows SelectableValues;

		// The property handle of FPulldownStructBase::SelectedValue.
		TSharedPtr<IPropertyHandle> SelectedValueHandle;

		// The property handle of FPulldownStructBase::CachedDisplayText.
		TSharedPtr<IPropertyHandle> CachedDisplayTextHandle;

		// The property handle of FPulldownStructBase::SearchableObject.
		TSharedPtr<IPropertyHandle> SearchableObjectHandle;

		// The property handle of FPulldownStructBase::bIsEdited.
		TSharedPtr<IPropertyHandle> IsEditedHandle;

		// The widget that displays a pull-down menu based on the SelectableValues.
		TSharedPtr<SPulldownSelectorComboButton> SelectedValueWidget;

		// A structure that represents the properties contained in a pull-down structure that need to be customized.
		struct FCustomizationProperties
		{
		public:
			// Constructor.
			FCustomizationProperties() = default;

			// Add the handle and name of the property that you need to customize.
			void Add(const FName& InPropertyName, TSharedPtr<IPropertyHandle>* InPropertyHandle);

			// Initializes the handles of property that need to be customized from the specified structure property handle.
			void Initialize(const TSharedRef<IPropertyHandle>& InStructPropertyHandle);

			// Returns whether all property handles that need to be customized have been initialized.
			bool IsInitialized() const;
			
			// Returns whether the specified property is included in the properties that need to be customized.
#if UE_4_25_OR_LATER
			bool Contains(FProperty* InProperty) const;
#else
			bool Contains(UProperty* InProperty) const;
#endif

			// Returns the number of properties that need to be customized.
			uint32 Num() const;

		private:
			// A map of the name and handle pointer for the property that needs to be customized.
			TMap<FName, TSharedPtr<IPropertyHandle>*> PropertyNamesToHandles;
		};
		FCustomizationProperties CustomizationProperties;
	};
}
