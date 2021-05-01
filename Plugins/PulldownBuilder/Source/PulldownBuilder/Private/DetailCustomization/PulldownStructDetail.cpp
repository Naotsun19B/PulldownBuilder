// Copyright 2021 Naotsun. All Rights Reserved.

#include "DetailCustomization/PulldownStructDetail.h"
#include "PulldownBuilderGlobals.h"
#include "Struct/PulldownStructType.h"
#include "Utility/PulldownBuilderUtils.h"
#include "Utility/SSearchableTextComboBox.h"
#include "PulldownStructBase.h"
#include "DetailWidgetRow.h"
#include "PropertyEditorModule.h"
#include "PropertyHandle.h"
#include "IDetailChildrenBuilder.h"
#include "HAL/PlatformApplicationMisc.h"

void FPulldownStructDetail::Register(const FPulldownStructType& StructType)
{
	if (StructType.IsEmpty())
	{
		UE_LOG(LogPulldownBuilder, Warning, TEXT("You have tried to register an empty struct..."));
		return;
	}
	
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout(
        *StructType,
        FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FPulldownStructDetail::MakeInstance)
    );
}

void FPulldownStructDetail::Unregister(const FPulldownStructType& StructType)
{
	if (StructType.IsEmpty())
	{
		UE_LOG(LogPulldownBuilder, Warning, TEXT("You have tried to unregister an empty struct..."));
		return;
	}

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomPropertyTypeLayout(
		*StructType
    );
}

TSharedRef<IPropertyTypeCustomization> FPulldownStructDetail::MakeInstance()
{
	return MakeShared<FPulldownStructDetail>();
}

void FPulldownStructDetail::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructPropertyHandle = InStructPropertyHandle;
	check(StructPropertyHandle);

	// Scan the properties of the structure for the property handle of FPulldownStructBase::SelectedValue.
	uint32 NumChildProperties;
	StructPropertyHandle->GetNumChildren(NumChildProperties);
	for (uint32 Index = 0; Index < NumChildProperties; Index++)
	{
		const TSharedPtr<IPropertyHandle> ChildPropertyHandle = StructPropertyHandle->GetChildHandle(Index);
		if (ChildPropertyHandle.IsValid())
		{
#if BEFORE_UE_4_24
			if (UProperty* ChildProperty = ChildPropertyHandle->GetProperty())
#else
			if (FProperty* ChildProperty = ChildPropertyHandle->GetProperty())
#endif
			{
				if (ChildProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue))
				{
					SelectedValueHandle = ChildPropertyHandle;
					break;
				}
			}
		}
	}

	// Do not register structures other than FPulldownStructBase in this detail customization.
	check(SelectedValueHandle.IsValid());
	
	HeaderRow.NameContent()
    [
        StructPropertyHandle->CreatePropertyNameWidget()
    ];

	// If the property is only FPulldownStructBase::SelectedValue, display it inline.
	if (NumChildProperties == 1 && !bNeverInlineDisplay)
	{
		HeaderRow.ValueContent()
			.MinDesiredWidth(500)
			[
				GenerateSelectableValuesWidget()
			];

		RebuildPulldown();
	}
}

void FPulldownStructDetail::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	check(StructPropertyHandle && SelectedValueHandle);

	// Add child properties other than FPulldownStructBase::SelectedValue to the StructBuilder.
	uint32 NumChildProperties;
	StructPropertyHandle->GetNumChildren(NumChildProperties);
	for (uint32 Index = 0; Index < NumChildProperties; Index++)
	{
		const TSharedPtr<IPropertyHandle> ChildPropertyHandle = StructPropertyHandle->GetChildHandle(Index);
		if (ChildPropertyHandle.IsValid())
		{
#if BEFORE_UE_4_24
			if (UProperty* ChildProperty = ChildPropertyHandle->GetProperty())
#else
			if (FProperty* ChildProperty = ChildPropertyHandle->GetProperty())
#endif
			{
				if (!IsCustomizationTarget(ChildProperty))
				{
					StructBuilder.AddProperty(ChildPropertyHandle.ToSharedRef());
					ChildPropertyHandle->SetOnPropertyValueChanged(
						FSimpleDelegate::CreateSP(this, &FPulldownStructDetail::RebuildPulldown)
					);
				}
			}
		}
	}

	// If there are multiple properties, do not display inline.
	if (NumChildProperties > 1 || bNeverInlineDisplay)
	{
		AddCustomRowBeforeSelectedValue(StructBuilder);
		
		StructBuilder.AddCustomRow(FText::FromName(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue)))
			.CopyAction(CreateSelectedValueCopyAction())
			.PasteAction(CreateSelectedValuePasteAction())
			.NameContent()
			[
				SelectedValueHandle->CreatePropertyNameWidget()
			]
			.ValueContent()
			.MinDesiredWidth(500)
			[
				GenerateSelectableValuesWidget()
			];

		AddCustomRowAfterSelectedValue(StructBuilder);

		RebuildPulldown();
	}
}

void FPulldownStructDetail::RebuildPulldown()
{
	check(StructPropertyHandle && SelectedValueHandle);

	// Find PulldownContents in the property structure and
	// build a list of strings to display in the pull-down menu.
    void* StructValueData = nullptr;
    FPropertyAccess::Result Result = StructPropertyHandle->GetValueData(StructValueData);
    if (Result == FPropertyAccess::Success)
    {	
        SelectableValues = GenerateSelectableValues();
    }
    // Empty the list if data acquisition fails or if multiple selections are made.
    else
    {
    	OnMutipleSelected();
        return;
    }

	RefreshPulldownWidget();
}

void FPulldownStructDetail::RefreshPulldownWidget()
{
	check(SelectedValueHandle);
	
	// Check if the currently set string is included in the constructed list.
	FName CurrentSelectedValue;
	SelectedValueHandle->GetValue(CurrentSelectedValue);

	TSharedPtr<FString> SelectedItem = FindSelectableValueByName(CurrentSelectedValue);
	if (!SelectedItem.IsValid())
	{
		SelectedValueHandle->SetValue(NAME_None);
		SelectedItem = FindSelectableValueByName(NAME_None);
	}

	if (SelectedValueWidget.IsValid())
	{
		SelectedValueWidget->RefreshOptions();
		SelectedValueWidget->SetSelectedItem(SelectedItem);
	}
}

TArray<TSharedPtr<FString>> FPulldownStructDetail::GenerateSelectableValues()
{
	check(StructPropertyHandle);
	
#if BEFORE_UE_4_24
	if (auto* StructProperty = CastField<UStructProperty>(StructPropertyHandle->GetProperty()))
#else
	if (auto* StructProperty = CastField<FStructProperty>(StructPropertyHandle->GetProperty()))
#endif
	{
		return FPulldownBuilderUtils::GetDisplayStringsFromStruct(StructProperty->Struct);
	}

	return FPulldownBuilderUtils::GetEmptyDisplayStrings();
}

void FPulldownStructDetail::OnMutipleSelected()
{
	SelectableValues.Reset();
}

bool FPulldownStructDetail::IsCustomizationTarget(FProperty* InProperty) const
{
	check(InProperty);
	return (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue));
}

TSharedRef<SWidget> FPulldownStructDetail::GenerateSelectableValuesWidget()
{
	return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			[
				SAssignNew(SelectedValueWidget, SSearchableTextComboBox)
				.OptionsSource(&SelectableValues)
				.OnSelectionChanged(this, &FPulldownStructDetail::OnSelectedValueChanged)
				.OnComboBoxOpening(this, &FPulldownStructDetail::RebuildPulldown)
			];
}

TSharedPtr<FString> FPulldownStructDetail::FindSelectableValueByName(const FName& InName) const
{
	const TSharedPtr<FString>* FoundItem = SelectableValues.FindByPredicate(
		[&](const TSharedPtr<FString>& Item)
		{
			return (Item.IsValid() && *Item == InName.ToString());
		});

	return (FoundItem != nullptr ? *FoundItem : nullptr);
}

void FPulldownStructDetail::OnSelectedValueChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo)
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
		
		UE_LOG(LogPulldownBuilder, Warning, TEXT("%s -> %s"), *OldSelectedValue.ToString(), *NewSelectedValue.ToString());
	}
}

FUIAction FPulldownStructDetail::CreateSelectedValueCopyAction()
{
	return FUIAction
	(
		FExecuteAction::CreateSP(this, &FPulldownStructDetail::OnSelectedValueCopyAction)
	);
}

FUIAction FPulldownStructDetail::CreateSelectedValuePasteAction()
{
	return FUIAction
	(
		FExecuteAction::CreateSP(this, & FPulldownStructDetail::OnSelectedValuePasteAction)
	);
}

void FPulldownStructDetail::OnSelectedValueCopyAction()
{
	if (!SelectedValueHandle.IsValid())
	{
		return;
	}

	FName SelectedValue;
	SelectedValueHandle->GetValue(SelectedValue);

	FPlatformApplicationMisc::ClipboardCopy(*SelectedValue.ToString());
}

void FPulldownStructDetail::OnSelectedValuePasteAction()
{
	if (!SelectedValueWidget.IsValid())
	{
		return;
	}
	
	FName PastedText;
	{
		FString ClipboardString;
		FPlatformApplicationMisc::ClipboardPaste(ClipboardString);
		PastedText = *ClipboardString;
	}

	TSharedPtr<FString> SelectedItem = FindSelectableValueByName(PastedText);
	if (SelectedItem.IsValid())
	{
		SelectedValueWidget->SetSelectedItem(SelectedItem);
	}
}
