// Copyright 2021 Naotsun. All Rights Reserved.

#include "CustomGraphPin/SPulldownStructGraphPin.h"
#include "PulldownBuilderGlobals.h"
#include "Common/PulldownBuilderUtils.h"
#include "PulldownStructBase.h"
#include "Widgets/Input/STextComboBox.h"

void SPulldownStructGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget> SPulldownStructGraphPin::GetDefaultValueWidget()
{
	RebuildPulldown();
	
	return SNew(SHorizontalBox)
			.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			[
				SAssignNew(PulldownWidget, STextComboBox)
				.OptionsSource(&DisplayStrings)
				.OnSelectionChanged(this, &SPulldownStructGraphPin::OnStateValueChanged)
				.InitiallySelectedItem(MakeShared<FString>(GetSelectedValueData().ToString()))
			];
}

void SPulldownStructGraphPin::RebuildPulldown()
{
	check(GraphPinObj);

	// Find Pulldown Contents in the property structure and
	// build a list of strings to display in the pull-down menu.
	if (auto* Struct = Cast<UScriptStruct>(GraphPinObj->PinType.PinSubCategoryObject))
	{
		DisplayStrings = FPulldownBuilderUtils::GetDisplayStringsFromStruct(Struct);
	}
	
	// Check if the currently set string is included in the constructed list.
	const FName& CurrentSelectedValue = GetSelectedValueData();
	TSharedPtr<FString> SelectedItem = FindDisplayNameByName(CurrentSelectedValue);
	if (!SelectedItem.IsValid())
	{
		SetSelectedValueData(NAME_None);
		SelectedItem = FindDisplayNameByName(NAME_None);
	}

	if (PulldownWidget.IsValid())
	{
		PulldownWidget->RefreshOptions();
		PulldownWidget->SetSelectedItem(SelectedItem);
	}
}

TSharedPtr<FString> SPulldownStructGraphPin::FindDisplayNameByName(const FName& InName) const
{
	const TSharedPtr<FString>* FoundItem = DisplayStrings.FindByPredicate(
        [&](const TSharedPtr<FString>& Item)
        {
            return (Item.IsValid() && *Item == InName.ToString());
        });

	return (FoundItem != nullptr ? *FoundItem : nullptr);
}

void SPulldownStructGraphPin::OnStateValueChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo)
{
	check(GraphPinObj);
	
	if (!SelectedItem.IsValid())
	{
		return;
	}

	SetSelectedValueData(**SelectedItem);
}

FName SPulldownStructGraphPin::GetSelectedValueData() const
{
	check(GraphPinObj);
	
	FString CurrentDefault = GraphPinObj->GetDefaultAsString();

	// If it is shorter than the variable name of FPulldownStructBase::SelectedValue, returns nullptr.
	const int32 MinLength = FString(GET_MEMBER_NAME_STRING_CHECKED(FPulldownStructBase, SelectedValue)).Len();
	if (CurrentDefault.Len() < MinLength)
	{
		return NAME_None;
	}

	// If there are multiple properties, only the FPulldownStructBase::SelectedValue part is extracted.
	if (CurrentDefault.Contains(TEXT(",")))
	{
		TArray<FString> ParsedString;
		CurrentDefault.ParseIntoArray(ParsedString, TEXT(","));

		const FString& FindTarget = FString::Printf(TEXT("%s="), GET_MEMBER_NAME_STRING_CHECKED(FPulldownStructBase, SelectedValue));
		const int32 FindTargetLength = FindTarget.Len();
		for (const auto& String : ParsedString)
		{
			bool bIsMatch = true;
			for (int32 Index = 0; Index < FindTargetLength; Index++)
			{
				if (!String.IsValidIndex(Index) ||
					FindTarget[Index] != String[Index])
				{
					bIsMatch = false;
					break;
				}
			}

			if (bIsMatch)
			{
				CurrentDefault = String;
				break;
			}
		}
	}
	
	// Extract only the value part of the structure string.
	FString VariableName;
	FString VariableValue;
	CurrentDefault.Split(TEXT("="), &VariableName, &VariableValue);
	CurrentDefault = VariableValue;

	CurrentDefault = CurrentDefault.Replace(TEXT(")"), TEXT(""));
	CurrentDefault = CurrentDefault.Replace(TEXT("\""), TEXT(""));

	return FName(CurrentDefault);
}

void SPulldownStructGraphPin::SetSelectedValueData(const FName& NewSelectedValue)
{
	check(GraphPinObj);
	
	if (NewSelectedValue != SelectedValue)
	{
		if (const UEdGraphSchema* Schema = GraphPinObj->GetSchema())
		{
			const FString& SelectedValueString = FString::Printf(
                TEXT("(%s=%s)"),
                GET_MEMBER_NAME_STRING_CHECKED(FPulldownStructBase, SelectedValue),
                *NewSelectedValue.ToString()
            );
			Schema->TrySetDefaultValue(*GraphPinObj, SelectedValueString);
		}
	}
}
