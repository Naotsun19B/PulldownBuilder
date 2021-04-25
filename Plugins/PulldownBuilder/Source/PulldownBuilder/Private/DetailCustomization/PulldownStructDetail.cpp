// Copyright 2021 Naotsun. All Rights Reserved.

#include "DetailCustomization/PulldownStructDetail.h"
#include "PulldownBuilderGlobals.h"
#include "Common/PulldownBuilderUtils.h"
#include "Common/PulldownStructType.h"
#include "PulldownStructBase.h"
#include "DetailWidgetRow.h"
#include "PropertyEditorModule.h"
#include "PropertyHandle.h"
#include "IDetailChildrenBuilder.h"
#include "Widgets/Input/STextComboBox.h"
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
}

void FPulldownStructDetail::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	check(StructPropertyHandle);

	// Add child properties other than FPulldownStructBase::SelectedValue to the Struct Builder.
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
				if (ChildProperty->GetFName() != GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue))
				{
					StructBuilder.AddProperty(ChildPropertyHandle.ToSharedRef());
					ChildPropertyHandle->SetOnPropertyValueChanged(
						FSimpleDelegate::CreateSP(this, &FPulldownStructDetail::RebuildPulldown)
					);
				}
			}
		}
	}

	check(SelectedValueHandle);

	StructBuilder.AddCustomRow(FText::FromName(GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue)))
		.CopyAction(CreateCopyAction())
		.PasteAction(CreatePasteAction())
		.NameContent()
		[
			SelectedValueHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(500)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			[
				SAssignNew(PulldownWidget, STextComboBox)
				.OptionsSource(&DisplayStrings)
				.OnSelectionChanged(this, &FPulldownStructDetail::OnStateValueChanged)
				.OnComboBoxOpening(this, &FPulldownStructDetail::RebuildPulldown)
			]
		];

	RebuildPulldown();
}

void FPulldownStructDetail::RebuildPulldown()
{
	if (StructPropertyHandle == nullptr ||
		!StructPropertyHandle->IsValidHandle() ||
		!SelectedValueHandle.IsValid()
	)
	{
		return;
	}

	// Find Pulldown Contents in the property structure and
	// build a list of strings to display in the pull-down menu.
#if BEFORE_UE_4_24
	if (auto* StructProperty = CastField<UStructProperty>(StructPropertyHandle->GetProperty()))
#else
	if (auto* StructProperty = CastField<FStructProperty>(StructPropertyHandle->GetProperty()))
#endif
	{	
		void* StructValueData = nullptr;
		FPropertyAccess::Result Result = StructPropertyHandle->GetValueData(StructValueData);

		if (Result == FPropertyAccess::Success)
		{	
			DisplayStrings = FPulldownBuilderUtils::GetDisplayStringsFromStruct(StructProperty->Struct);
		}
		// Empty the list if data acquisition fails or if multiple selections are made.
		else
		{
			DisplayStrings.Reset();
			return;
		}
	}

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

TSharedPtr<FString> FPulldownStructDetail::FindDisplayStringByName(const FName& InName) const
{
	const TSharedPtr<FString>* FoundItem = DisplayStrings.FindByPredicate(
		[&](const TSharedPtr<FString>& Item)
		{
			return (Item.IsValid() && *Item == InName.ToString());
		});

	return (FoundItem != nullptr ? *FoundItem : nullptr);
}

void FPulldownStructDetail::OnStateValueChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo)
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

FUIAction FPulldownStructDetail::CreateCopyAction()
{
	return FUIAction
	(
		FExecuteAction::CreateSP(this, &FPulldownStructDetail::OnCopyAction)
	);
}

FUIAction FPulldownStructDetail::CreatePasteAction()
{
	return FUIAction
	(
		FExecuteAction::CreateSP(this, & FPulldownStructDetail::OnPasteAction)
	);
}

void FPulldownStructDetail::OnCopyAction()
{
	if (!SelectedValueHandle.IsValid())
	{
		return;
	}

	FName SelectedValue;
	SelectedValueHandle->GetValue(SelectedValue);

	FPlatformApplicationMisc::ClipboardCopy(*SelectedValue.ToString());
}

void FPulldownStructDetail::OnPasteAction()
{
	if (!PulldownWidget.IsValid())
	{
		return;
	}
	
	FName PastedText;
	{
		FString ClipboardString;
		FPlatformApplicationMisc::ClipboardPaste(ClipboardString);
		PastedText = *ClipboardString;
	}

	TSharedPtr<FString> SelectedItem = FindDisplayStringByName(PastedText);
	if (SelectedItem.IsValid())
	{
		PulldownWidget->SetSelectedItem(SelectedItem);
	}
}
