// Copyright 2021 Naotsun. All Rights Reserved.

#include "DetailCustomization/PulldownStructDetailBase.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include "IDetailChildrenBuilder.h"
#include "Widgets/Input/STextComboBox.h"

void FPulldownStructDetailBase::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	unimplemented();
}

void FPulldownStructDetailBase::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	unimplemented();
}

void FPulldownStructDetailBase::RebuildPulldown()
{
	// Check if the currently set string is included in the constructed list.
	FName CurrentSelectedValue;
	SelectedValueHandle->GetValue(CurrentSelectedValue);

	TSharedPtr<FString> SelectedItem = FindDisplayStringByName(CurrentSelectedValue);
	if (!SelectedItem.IsValid())
	{
		SelectedValueHandle->SetValue(NAME_None);
		SelectedItem = FindDisplayStringByName(NAME_None);
	}

	if (PulldownWidget.IsValid())
	{
		PulldownWidget->RefreshOptions();
		PulldownWidget->SetSelectedItem(SelectedItem);
	}
}

TSharedPtr<FString> FPulldownStructDetailBase::FindDisplayStringByName(const FName& InName) const
{
	const TSharedPtr<FString>* FoundItem = DisplayStrings.FindByPredicate(
		[&](const TSharedPtr<FString>& Item)
		{
			return (Item.IsValid() && *Item == InName.ToString());
		});

	return (FoundItem != nullptr ? *FoundItem : nullptr);
}

void FPulldownStructDetailBase::OnStateValueChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo)
{
	if (!SelectedItem.IsValid() || !SelectedValueHandle.IsValid())
	{
		return;
	}

	FName NewSelectedValue = **SelectedItem;
	FName OldSelectedValue;
	SelectedValueHandle->GetValue(OldSelectedValue);
	if (NewSelectedValue != OldSelectedValue)
	{
		SelectedValueHandle->SetValue(NewSelectedValue);
	}
}
