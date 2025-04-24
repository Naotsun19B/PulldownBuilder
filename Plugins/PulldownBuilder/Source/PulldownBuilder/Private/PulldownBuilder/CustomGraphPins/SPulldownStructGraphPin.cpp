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
		RebuildPulldown();

		return GenerateSelectableValuesWidget();
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
		const TSharedPtr<FName> CurrentSelectedValue = GetPropertyValue(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue));
		TSharedPtr<FPulldownRow> SelectedItem = nullptr;
		if (CurrentSelectedValue.IsValid())
		{
			SelectedItem = FindSelectableValueByName(*CurrentSelectedValue);
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

	TArray<TSharedPtr<FPulldownRow>> SPulldownStructGraphPin::GenerateSelectableValues()
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

		return FPulldownBuilderUtils::GetEmptyPulldownRows();
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
		const TSharedPtr<FPulldownRow>* FoundItem = SelectableValues.FindByPredicate(
			[&](const TSharedPtr<FPulldownRow>& Item)
			{
				return (Item.IsValid() && Item->SelectedValue == InName.ToString());
			});

		return (FoundItem != nullptr ? *FoundItem : nullptr);
	}

	TSharedPtr<FPulldownRow> SPulldownStructGraphPin::GetSelection() const
	{
		const TSharedPtr<FName> SelectedValue = GetPropertyValue(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue));
		const FName& NameToFind = (SelectedValue.IsValid() ? *SelectedValue : NAME_None);
		return FindSelectableValueByName(NameToFind);
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
		const TSharedPtr<FName> CurrentSelectedValue = GetPropertyValue(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue));
		if (SelectedItem.IsValid() && CurrentSelectedValue.IsValid())
		{
			if (SelectedItem->SelectedValue != CurrentSelectedValue->ToString())
			{
				SetPropertyValue(
					GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue),
					*SelectedItem->SelectedValue
				);
			}
		}
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
		
		FName NewSearchableObject = NAME_None;
		if (const UPulldownContents* RelatedPulldownContents = GetRelatedPulldownContents())
		{
			const FAssetData RelatedPulldownContentsAssetData(RelatedPulldownContents);
			NewSearchableObject = *RelatedPulldownContentsAssetData.GetExportTextName();
		}

		const TSharedPtr<FName>& SearchableObject = GetPropertyValue(FPulldownStructBase::SearchableObjectPropertyName);
		if (!SearchableObject.IsValid() || (*SearchableObject != NewSearchableObject))
		{
			SetPropertyValue(
				FPulldownStructBase::SearchableObjectPropertyName,
				NewSearchableObject
			);
		}
	}

	TSharedPtr<FName> SPulldownStructGraphPin::GetPropertyValue(const FName& PropertyName) const
	{
		check(GraphPinObj != nullptr);
	
		return FPulldownBuilderUtils::StructStringToMemberValue(
			GraphPinObj->GetDefaultAsString(),
			PropertyName
		);
	}

	void SPulldownStructGraphPin::SetPropertyValue(const FName& PropertyName, const FName& NewPropertyValue)
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
}
