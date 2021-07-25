﻿// Copyright 2021 Naotsun. All Rights Reserved.

#include "CustomGraphPins/SNativeLessPulldownStructGraphPin.h"
#include "Assets/PulldownContents.h"
#include "Utilities/PulldownBuilderUtils.h"
#include "Widgets/SSearchableTextComboBox.h"
#include "NativeLessPulldownStruct.h"

void SNativeLessPulldownStructGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SPulldownStructGraphPin::Construct(SPulldownStructGraphPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget> SNativeLessPulldownStructGraphPin::GetDefaultValueWidget()
{
	RebuildPulldown();

	const TSharedPtr<FName> PulldownSource = GetPropertyValue(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource));
	const FName& NameToFind = (PulldownSource.IsValid() ? *PulldownSource : NAME_None);
	
	return SNew(SHorizontalBox)
			.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SHorizontalBox)
				.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(PulldownSourceWidget, SSearchableTextComboBox)
					.OptionsSource(&PulldownContentsNames)
					.OnSelectionChanged(this, &SNativeLessPulldownStructGraphPin::OnPulldownSourceChanged)
					.OnComboBoxOpening(this, &SNativeLessPulldownStructGraphPin::RebuildPulldown)
					.InitiallySelectedItem(FindPulldownContentsNameByName(NameToFind))
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				GenerateSelectableValuesWidget()
			];
}

void SNativeLessPulldownStructGraphPin::RefreshPulldownWidget()
{
	// Check if the currently set string is included in the constructed list.
	const TSharedPtr<FName>& CurrentPulldownSource = GetPropertyValue(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource));
	TSharedPtr<FString> SelectedItem = nullptr;
	if (CurrentPulldownSource.IsValid())
	{
		SelectedItem = FindPulldownContentsNameByName(*CurrentPulldownSource);
	}
	if (!SelectedItem.IsValid())
	{
		SetPropertyValue(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource), NAME_None);
		SelectedItem = FindPulldownContentsNameByName(NAME_None);
	}

	if (PulldownSourceWidget.IsValid())
	{
		PulldownSourceWidget->RefreshOptions();
		PulldownSourceWidget->SetSelectedItem(SelectedItem);
	}

	SPulldownStructGraphPin::RefreshPulldownWidget();
}

TArray<TSharedPtr<FString>> SNativeLessPulldownStructGraphPin::GenerateSelectableValues()
{
	PulldownContentsNames.Reset();
	PulldownContentsNames.Add(MakeShared<FString>(FName(NAME_None).ToString()));

	const TArray<UPulldownContents*>& AllPulldownContents = FPulldownBuilderUtils::GetAllPulldownContents();
	for (const auto& PulldownContents : AllPulldownContents)
	{
		if (IsValid(PulldownContents))
		{
			PulldownContentsNames.Add(MakeShared<FString>(PulldownContents->GetName()));
		}
	}

	const TSharedPtr<FName> PulldownSource = GetPropertyValue(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource));
	if (PulldownSource.IsValid())
	{
		if (UPulldownContents* SourceAsset = FPulldownBuilderUtils::FindPulldownContentsByName(*PulldownSource))
		{
			return SourceAsset->GetDisplayStrings();
		}
	}

	return FPulldownBuilderUtils::GetEmptyDisplayStrings();
}

TSharedPtr<FString> SNativeLessPulldownStructGraphPin::FindPulldownContentsNameByName(const FName& InName) const
{
	const TSharedPtr<FString>* FoundItem = PulldownContentsNames.FindByPredicate(
		[&](const TSharedPtr<FString>& Item)
		{
			return (Item.IsValid() && *Item == InName.ToString());
		});

	return (FoundItem != nullptr ? *FoundItem : nullptr);
}

void SNativeLessPulldownStructGraphPin::OnPulldownSourceChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo)
{
	TSharedPtr<FName> CurrentPulldownSource = GetPropertyValue(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource));
	if (SelectedItem.IsValid() && CurrentPulldownSource.IsValid())
	{
		if (*SelectedItem != CurrentPulldownSource->ToString())
		{
			SetPropertyValue(
			GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource),
			**SelectedItem
			);

			// Since the base asset of the pull-down menu has changed, set SelectedValue to None.
			SetPropertyValue(
				GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue),
				NAME_None
			);
			SPulldownStructGraphPin::RefreshPulldownWidget();
		}	
	}
}