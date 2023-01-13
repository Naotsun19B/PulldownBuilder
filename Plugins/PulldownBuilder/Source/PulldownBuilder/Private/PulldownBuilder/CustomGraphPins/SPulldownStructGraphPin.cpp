// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownBuilder/CustomGraphPins/SPulldownStructGraphPin.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownBuilder/Widgets/SPulldownSelectorComboButton.h"
#include "PulldownBuilder/Types/PulldownRow.h"
#include "PulldownBuilder/Types/StructContainer.h"
#include "PulldownStruct/PulldownStructBase.h"

namespace PulldownBuilder
{
	void SPulldownStructGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
	{
		SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
	}

	TSharedRef<SWidget> SPulldownStructGraphPin::GetDefaultValueWidget()
	{
		RebuildPulldown();

		return GenerateSelectableValuesWidget();
	}

	void SPulldownStructGraphPin::RebuildPulldown()
	{
		// Find Pulldown Contents in the property structure and
		// build a list of strings to display in the pull-down menu.
		SelectableValues = GenerateSelectableValues();

		RefreshPulldownWidget();
	}

	void SPulldownStructGraphPin::RefreshPulldownWidget()
	{	
		// Check if the currently set string is included in the constructed list.
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
		FStructContainer StructContainer;
		if (GenerateStructContainer(StructContainer))
		{
			return FPulldownBuilderUtils::GetPulldownRowsFromStruct(
				StructContainer.GetScriptStruct(),
				TArray<UObject*>{ GetOuterAsset() },
				StructContainer
			);
		}

		return FPulldownBuilderUtils::GetEmptyPulldownRows();
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
				.OnSelectionChanged(this, &SPulldownStructGraphPin::OnSelectedValueChanged)
				.OnComboBoxOpened(this, &SPulldownStructGraphPin::RebuildPulldown)
			];
	}

	TSharedPtr<FPulldownRow> SPulldownStructGraphPin::FindSelectableValueByName(const FName& InName) const
	{
		const TSharedPtr<FPulldownRow>* FoundItem = SelectableValues.FindByPredicate(
			[&](const TSharedPtr<FPulldownRow>& Item)
			{
				return (Item.IsValid() && Item->DisplayText.ToString() == InName.ToString());
			});

		return (FoundItem != nullptr ? *FoundItem : nullptr);
	}

	TSharedPtr<FPulldownRow> SPulldownStructGraphPin::GetSelection() const
	{
		const TSharedPtr<FName> SelectedValue = GetPropertyValue(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue));
		const FName& NameToFind = (SelectedValue.IsValid() ? *SelectedValue : NAME_None);
		return FindSelectableValueByName(NameToFind);
	}

	void SPulldownStructGraphPin::OnSelectedValueChanged(TSharedPtr<FPulldownRow> SelectedItem, ESelectInfo::Type SelectInfo)
	{
		const TSharedPtr<FName> CurrentSelectedValue = GetPropertyValue(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue));
		if (SelectedItem.IsValid() && CurrentSelectedValue.IsValid())
		{
			if (*SelectedItem != CurrentSelectedValue->ToString())
			{
				SetPropertyValue(
					GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue),
					*SelectedItem->DisplayText.ToString()
				);
			}
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
			// Set the created value for the pin.
			const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
			check(IsValid(Schema));
			Schema->TrySetDefaultValue(*GraphPinObj, *NewDefaultValue);
		}
	}

	UObject* SPulldownStructGraphPin::GetOuterAsset() const
	{
		UObject* CurrentObject = GraphPinObj->GetOuter();
		while (IsValid(CurrentObject))
		{
			if (CurrentObject->IsAsset())
			{
				return CurrentObject;
			}

			CurrentObject = CurrentObject->GetOuter();
		}

		return nullptr;
	}

	bool SPulldownStructGraphPin::GenerateStructContainer(FStructContainer& StructContainer) const
	{
		check(GraphPinObj != nullptr);

		const auto* ScriptStruct = Cast<UScriptStruct>(GraphPinObj->PinType.PinSubCategoryObject);
		if (!IsValid(ScriptStruct))
		{
			return false;
		}
		
		auto* RawData = static_cast<uint8*>(FMemory::Malloc(ScriptStruct->GetStructureSize()));

		const bool bWasSuccessful = FPulldownBuilderUtils::GetStructRawDataFromDefaultValueString(
			ScriptStruct,
			GraphPinObj->GetDefaultAsString(),
			RawData
		);
		if (bWasSuccessful)
		{
			StructContainer = FStructContainer(ScriptStruct, RawData);
		}

		FMemory::Free(RawData);

		return bWasSuccessful;
	}
}
