// Copyright 2021 Naotsun. All Rights Reserved.

#include "CustomGraphPin/SPulldownStructGraphPin.h"
#include "PulldownBuilderGlobals.h"
#include "Utility/PulldownBuilderUtils.h"
#include "Utility/SSearchableTextComboBox.h"
#include "PulldownStructBase.h"

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
	TSharedPtr<FString> SelectedItem = nullptr;
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
		SelectedValueWidget->RefreshOptions();
		SelectedValueWidget->SetSelectedItem(SelectedItem);
	}
}

TArray<TSharedPtr<FString>> SPulldownStructGraphPin::GenerateSelectableValues()
{
	check(GraphPinObj);
	
	if (auto* Struct = Cast<UScriptStruct>(GraphPinObj->PinType.PinSubCategoryObject))
	{
		return FPulldownBuilderUtils::GetDisplayStringsFromStruct(Struct);
	}

	return FPulldownBuilderUtils::GetEmptyDisplayStrings();
}

TSharedRef<SWidget> SPulldownStructGraphPin::GenerateSelectableValuesWidget()
{
    const TSharedPtr<FName> SelectedValue = GetPropertyValue(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue));
	const FName& NameToFind = (SelectedValue.IsValid() ? *SelectedValue : NAME_None);
	
	return SNew(SHorizontalBox)
			.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(SelectedValueWidget, SSearchableTextComboBox)
				.OptionsSource(&SelectableValues)
				.OnSelectionChanged(this, &SPulldownStructGraphPin::OnSelectedValueChanged)
				.OnComboBoxOpening(this, &SPulldownStructGraphPin::RebuildPulldown)
				.InitiallySelectedItem(FindSelectableValueByName(NameToFind))
			];
}

TSharedPtr<FString> SPulldownStructGraphPin::FindSelectableValueByName(const FName& InName) const
{
	const TSharedPtr<FString>* FoundItem = SelectableValues.FindByPredicate(
        [&](const TSharedPtr<FString>& Item)
        {
            return (Item.IsValid() && *Item == InName.ToString());
        });

	return (FoundItem != nullptr ? *FoundItem : nullptr);
}

void SPulldownStructGraphPin::OnSelectedValueChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo)
{
	TSharedPtr<FName> CurrentSelectedValue = GetPropertyValue(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue));
	if (SelectedItem.IsValid() && CurrentSelectedValue.IsValid())
	{
		if (*SelectedItem != CurrentSelectedValue->ToString())
		{
			SetPropertyValue(
			GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue),
			**SelectedItem
			);
		}
	}
}

TSharedPtr<FName> SPulldownStructGraphPin::GetPropertyValue(const FName& PropertyName) const
{
	check(GraphPinObj);
	
	return FPulldownBuilderUtils::StructStringToMemberValue(
		GraphPinObj->GetDefaultAsString(),
		PropertyName
	);
}

void SPulldownStructGraphPin::SetPropertyValue(const FName& PropertyName, const FName& NewPropertyValue)
{
	check(GraphPinObj);

	const TSharedPtr<FString> NewDefaultValue = FPulldownBuilderUtils::MemberValueToStructString(
		GraphPinObj->GetDefaultAsString(),
		PropertyName,
		NewPropertyValue
	);
	if (NewDefaultValue.IsValid())
	{
		// Set the created value for the pin.
		const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
		check(Schema);
		Schema->TrySetDefaultValue(*GraphPinObj, *NewDefaultValue);
	}
}
