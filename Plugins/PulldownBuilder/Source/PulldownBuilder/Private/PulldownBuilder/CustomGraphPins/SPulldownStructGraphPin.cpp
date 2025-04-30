// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/CustomGraphPins/SPulldownStructGraphPin.h"
#include "PulldownBuilder/CustomGraphPins/GraphPinContextMenuExtender.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownBuilder/Widgets/SPulldownSelectorComboButton.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Types/PulldownRow.h"
#include "PulldownBuilder/Types/StructContainer.h"
#include "PulldownStruct/PulldownStructBase.h"

namespace PulldownBuilder
{
	void SPulldownStructGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
	{
		SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);

		FGraphPinContextMenuExtender::OnPinDefaultValueChanged.AddRaw(this, &SPulldownStructGraphPin::RebuildPulldown);
	}

	SPulldownStructGraphPin::~SPulldownStructGraphPin()
	{
		FGraphPinContextMenuExtender::OnPinDefaultValueChanged.RemoveAll(this);
	}

	TSharedRef<SWidget> SPulldownStructGraphPin::GetDefaultValueWidget()
	{
		InitializePulldown();

		return GenerateSelectableValuesWidget();
	}

	void SPulldownStructGraphPin::InitializePulldown()
	{
		UpdateSearchableObject();

		// If a default value is set and the selected value is either None or other than the default value, marks it as edited.
		if (!IsEdited())
		{
			bool bNeedToMarkEdited = true;
			FName CurrentSelectedValue;
			if (GetPropertyValue(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue), CurrentSelectedValue))
			{
				const TSharedPtr<FPulldownRow> DefaultRow = SelectableValues.GetDefaultRow();
				const FName DefaultValue = (DefaultRow.IsValid() ? FName(*DefaultRow->SelectedValue) : NAME_None);
				if (CurrentSelectedValue == DefaultValue)
				{
					bNeedToMarkEdited = false;
				}
			}
			if (bNeedToMarkEdited)
			{
				SetPropertyValue(FPulldownStructBase::IsEditedPropertyName, true);
			}
		}
		
		// If the value has not been edited during the initialization, the default value is applied.
		if (!IsEdited())
		{
			ApplyDefaultValue();
		}
		
		RefreshPulldownWidget();
	}

	void SPulldownStructGraphPin::RebuildPulldown()
	{
		// Finds Pulldown Contents in the property struct and
		// build a list of strings to display in the pull-down menu.
		SelectableValues = GenerateSelectableValues();

		UpdateSearchableObject();
		RefreshPulldownWidget();
	}

	void SPulldownStructGraphPin::RefreshPulldownWidget()
	{	
		// Checks if the currently set string is included in the constructed list.
		TSharedPtr<FPulldownRow> SelectedItem = nullptr;
		
		FName CurrentSelectedValue;
		if (GetPropertyValue(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue), CurrentSelectedValue))
		{
			SelectedItem = FindSelectableValueByName(CurrentSelectedValue);
		}
		
		if (!SelectedItem.IsValid())
		{
			SetPropertyValue(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue), NAME_None);
			SelectedItem = FindSelectableValueByName(NAME_None);
		}

		if (SelectedValueWidget.IsValid())
		{
			SelectedValueWidget->RefreshList();
			SelectedValueWidget->SetSelectedItem(SelectedItem);
		}
	}

	FPulldownRows SPulldownStructGraphPin::GenerateSelectableValues()
	{
		check(GraphPinObj != nullptr);
		
		FStructContainer StructContainer;
		if (FPulldownBuilderUtils::GenerateStructContainerFromPin(GraphPinObj, StructContainer))
		{
			return FPulldownBuilderUtils::GetPulldownRowsFromStruct(
				StructContainer.GetScriptStruct(),
				TArray<UObject*>{ FPulldownBuilderUtils::GetOuterAssetFromPin(GraphPinObj) },
				StructContainer
			);
		}

		return FPulldownRows::Empty;
	}

	UPulldownContents* SPulldownStructGraphPin::GetRelatedPulldownContents() const
	{
		if (const UScriptStruct* RelatedPulldownStructType = GetRelatedPulldownStructType())
		{
			return FPulldownBuilderUtils::FindPulldownContentsByStruct(RelatedPulldownStructType);
		}

		return nullptr;
	}

	const UScriptStruct* SPulldownStructGraphPin::GetRelatedPulldownStructType() const
	{
		check(GraphPinObj != nullptr);
		
		FStructContainer StructContainer;
		if (FPulldownBuilderUtils::GenerateStructContainerFromPin(GraphPinObj, StructContainer))
		{
			return StructContainer.GetScriptStruct();
		}

		return nullptr;
	}

	TSharedRef<SWidget> SPulldownStructGraphPin::GenerateSelectableValuesWidget()
	{
		return
			SNew(SHorizontalBox)
			.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(SelectedValueWidget, SPulldownSelectorComboButton)
				.ListItemsSource(&SelectableValues)
				.GetSelection(this, &SPulldownStructGraphPin::GetSelection)
				.HeightOverride(this, &SPulldownStructGraphPin::GetIndividualPanelHeight)
				.WidthOverride(this, &SPulldownStructGraphPin::GetIndividualPanelWidth)
				.OnSelectionChanged(this, &SPulldownStructGraphPin::OnSelectedValueChanged)
				.OnComboBoxOpened(this, &SPulldownStructGraphPin::RebuildPulldown)
			];
	}

	TSharedPtr<FPulldownRow> SPulldownStructGraphPin::FindSelectableValueByName(const FName& InName) const
	{
		return SelectableValues.FindByPredicate(
			[&](const TSharedPtr<FPulldownRow>& Row)
			{
				return (Row.IsValid() && Row->SelectedValue.Equals(InName.ToString()));
			}
		);
	}

	TSharedPtr<FPulldownRow> SPulldownStructGraphPin::GetSelection() const
	{
		FName SelectedValue;
		if (!GetPropertyValue(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue), SelectedValue))
		{
			return nullptr;
		}
		
		return FindSelectableValueByName(SelectedValue);
	}

	float SPulldownStructGraphPin::GetIndividualPanelHeight() const
	{
		if (const UPulldownContents* PulldownContents = GetRelatedPulldownContents())
		{
			const TOptional<FVector2D>& IndividualPanelSize = PulldownContents->GetIndividualPanelSize();
			if (IndividualPanelSize.IsSet())
			{
				return IndividualPanelSize.GetValue().Y;
			}
		}

		return 0.f;
	}

	float SPulldownStructGraphPin::GetIndividualPanelWidth() const
	{
		if (const UPulldownContents* PulldownContents = GetRelatedPulldownContents())
		{
			const TOptional<FVector2D>& IndividualPanelSize = PulldownContents->GetIndividualPanelSize();
			if (IndividualPanelSize.IsSet())
			{
				return IndividualPanelSize.GetValue().X;
			}
		}

		return 0.f;
	}

	void SPulldownStructGraphPin::OnSelectedValueChanged(TSharedPtr<FPulldownRow> SelectedItem, ESelectInfo::Type SelectInfo)
	{
		if (!SelectedItem.IsValid())
		{
			return;
		}

		FName CurrentSelectedValue;
		if (!GetPropertyValue(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue), CurrentSelectedValue))
		{
			return;
		}

		if (*SelectedItem->SelectedValue == CurrentSelectedValue)
		{
			return;
		}

		SetPropertyValue(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue), SelectedItem->SelectedValue);
		SetPropertyValue(FPulldownStructBase::IsEditedPropertyName, true);
	}

	void SPulldownStructGraphPin::UpdateSearchableObject()
	{
		const UScriptStruct* RelatedPulldownStructType = GetRelatedPulldownStructType();
		if (!IsValid(RelatedPulldownStructType))
		{
			return;
		}

		if (!FPulldownBuilderUtils::HasPulldownStructPostSerialize(RelatedPulldownStructType))
		{
			return;
		}

		FName SearchableObject;
		if (!GetPropertyValue(FPulldownStructBase::SearchableObjectPropertyName, SearchableObject))
		{
			return;
		}
		
		FName NewSearchableObject = NAME_None;
		if (const UPulldownContents* RelatedPulldownContents = GetRelatedPulldownContents())
		{
			const FAssetData RelatedPulldownContentsAssetData(RelatedPulldownContents);
			NewSearchableObject = *RelatedPulldownContentsAssetData.GetExportTextName();
		}
		if (SearchableObject != NewSearchableObject)
		{
			SetPropertyValue(FPulldownStructBase::SearchableObjectPropertyName, NewSearchableObject);
		}
	}

	void SPulldownStructGraphPin::ApplyDefaultValue(const bool bForceApply)
	{
		if (!bForceApply && IsEdited())
		{
			return;
		}
		
		const TSharedPtr<FPulldownRow> DefaultRow = SelectableValues.GetDefaultRow();
		const FName DefaultValue = (DefaultRow.IsValid() ? FName(*DefaultRow->SelectedValue) : FName(NAME_None));
		SetPropertyValue(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue), DefaultValue);
		SetPropertyValue(FPulldownStructBase::IsEditedPropertyName, false);
	}

	bool SPulldownStructGraphPin::IsEdited() const
	{
		bool IsEdited;
		return (GetPropertyValue(FPulldownStructBase::IsEditedPropertyName, IsEdited) && IsEdited);
	}

	bool SPulldownStructGraphPin::GetPropertyValue(const FName& PropertyName, const TFunction<void(const FString& PropertyValue)>& Predicate) const
	{
		const TSharedPtr<FString> MemberValue = FPulldownBuilderUtils::StructStringToMemberValue(GraphPinObj->GetDefaultAsString(), PropertyName);
		if (!MemberValue.IsValid())
		{
			return false;
		}

		Predicate(*MemberValue);
		return true;
	}

	bool SPulldownStructGraphPin::GetPropertyValue(const FName& PropertyName, FString& StringPropertyValue) const
	{
		return GetPropertyValue(PropertyName, [&](const FString& PropertyValue) { StringPropertyValue = PropertyValue; });
	}

	bool SPulldownStructGraphPin::GetPropertyValue(const FName& PropertyName, FName& NamePropertyValue) const
	{
		return GetPropertyValue(PropertyName, [&](const FString& PropertyValue) { NamePropertyValue = *PropertyValue; });
	}

	bool SPulldownStructGraphPin::GetPropertyValue(const FName& PropertyName, bool& bBoolPropertyValue) const
	{
		return GetPropertyValue(PropertyName, [&](const FString& PropertyValue) { bBoolPropertyValue = PropertyValue.ToBool(); });
	}

	void SPulldownStructGraphPin::SetPropertyValue(const FName& PropertyName, const FString& NewPropertyValue)
	{
		check(GraphPinObj != nullptr);

		const TSharedPtr<FString> NewDefaultValue = FPulldownBuilderUtils::MemberValueToStructString(
			GraphPinObj->GetDefaultAsString(),
			PropertyName,
			NewPropertyValue
		);
		if (NewDefaultValue.IsValid())
		{
			// Sets the created value for the pin.
			const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
			check(IsValid(Schema));
			Schema->TrySetDefaultValue(*GraphPinObj, *NewDefaultValue);
		}
	}

	void SPulldownStructGraphPin::SetPropertyValue(const FName& PropertyName, const FName& NewNamePropertyValue)
	{
		SetPropertyValue(PropertyName, NewNamePropertyValue.ToString());
	}

	void SPulldownStructGraphPin::SetPropertyValue(const FName& PropertyName, const bool bNewBoolPropertyValue)
	{
		SetPropertyValue(PropertyName, FName(bNewBoolPropertyValue ? TEXT("True") : TEXT("False")));
	}
}
