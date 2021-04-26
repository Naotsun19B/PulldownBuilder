// Copyright 2021 Naotsun. All Rights Reserved.

#include "DetailCustomization/NativeLessPulldownStructDetail.h"
#include "PulldownBuilderGlobals.h"
#include "Asset/PulldownContents.h"
#include "Utility/PulldownBuilderUtils.h"
#include "NativeLessPulldownStruct.h"
#include "DetailWidgetRow.h"
#include "PropertyEditorModule.h"
#include "PropertyHandle.h"
#include "IDetailChildrenBuilder.h"
#include "Widgets/Input/STextComboBox.h"
#include "HAL/PlatformApplicationMisc.h"

void FNativeLessPulldownStructDetail::Register()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout(
        *FNativeLessPulldownStruct::StaticStruct()->GetName(),
        FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FNativeLessPulldownStructDetail::MakeInstance)
    );
}

void FNativeLessPulldownStructDetail::Unregister()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomPropertyTypeLayout(
		*FNativeLessPulldownStruct::StaticStruct()->GetName()
    );
}

TSharedRef<IPropertyTypeCustomization> FNativeLessPulldownStructDetail::MakeInstance()
{
	return MakeShared<FNativeLessPulldownStructDetail>();
}

void FNativeLessPulldownStructDetail::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructPropertyHandle = InStructPropertyHandle;
	check(StructPropertyHandle);

	// Scan the properties of the structure for the property handle of FPulldownStructBase::SelectedValue
	// and FNativeLessPulldownStruct::PulldownContentsName.
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
				}
				else if (ChildProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource))
				{
					PulldownSourceHandle = ChildPropertyHandle;
				}
			}
		}
	}

	// Do not register structures other than FNativeLessPulldownStruct in this detail customization.
	check(SelectedValueHandle.IsValid() && PulldownSourceHandle.IsValid());
	
	HeaderRow.NameContent()
    [
        StructPropertyHandle->CreatePropertyNameWidget()
    ];
}

void FNativeLessPulldownStructDetail::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
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
				if (ChildProperty->GetFName() != GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue) &&
					ChildProperty->GetFName() != GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource)
				)
				{
					StructBuilder.AddProperty(ChildPropertyHandle.ToSharedRef());
					ChildPropertyHandle->SetOnPropertyValueChanged(
						FSimpleDelegate::CreateSP(this, &FNativeLessPulldownStructDetail::RebuildPulldown)
					);
				}
			}
		}
	}

	check(SelectedValueHandle.IsValid() && PulldownSourceHandle.IsValid());

	StructBuilder.AddCustomRow(FText::FromName(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource)))
		.CopyAction(CreateCopyAction())
		.PasteAction(CreatePasteAction())
		.NameContent()
		[
			PulldownSourceHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(500)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			[
				SAssignNew(SourceAssetPulldownWidget, STextComboBox)
				.OptionsSource(&PulldownContentsNames)
				.OnSelectionChanged(this, &FNativeLessPulldownStructDetail::OnSourceAssetChanged)
				.OnComboBoxOpening(this, &FNativeLessPulldownStructDetail::RebuildPulldown)
			]
		];

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
				.OnSelectionChanged(this, &FNativeLessPulldownStructDetail::OnStateValueChanged)
				.OnComboBoxOpening(this, &FNativeLessPulldownStructDetail::RebuildPulldown)
			]
		];

	RebuildPulldown();
}

void FNativeLessPulldownStructDetail::RebuildPulldown()
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

			DisplayStrings.Reset();
			FName PulldownSource;
			PulldownSourceHandle->GetValue(PulldownSource);
			if (UPulldownContents* SourceAsset = FPulldownBuilderUtils::FindPulldownContentsByName(PulldownSource))
			{
				DisplayStrings = SourceAsset->GetDisplayStrings();
			}
			else
			{
				DisplayStrings.Add(MakeShared<FString>(FName(NAME_None).ToString()));
			}
		}
		// Empty the list if data acquisition fails or if multiple selections are made.
		else
		{
			PulldownContentsNames.Reset();
			DisplayStrings.Reset();
			return;
		}
	}

	// Check if the currently set string is included in the constructed list.
	FName CurrentPulldownContentsName;
	PulldownSourceHandle->GetValue(CurrentPulldownContentsName);

	TSharedPtr<FString> SelectedItem = FindPulldownContentsNameByName(CurrentPulldownContentsName);
	if (!SelectedItem.IsValid())
	{
		PulldownSourceHandle->SetValue(NAME_None);
		SelectedItem = FindPulldownContentsNameByName(NAME_None);
	}

	if (SourceAssetPulldownWidget.IsValid())
	{
		SourceAssetPulldownWidget->RefreshOptions();
		SourceAssetPulldownWidget->SetSelectedItem(SelectedItem);
	}

	FPulldownStructDetailBase::RebuildPulldown();
}

TSharedPtr<FString> FNativeLessPulldownStructDetail::FindPulldownContentsNameByName(const FName& InName) const
{
	const TSharedPtr<FString>* FoundItem = PulldownContentsNames.FindByPredicate(
		[&](const TSharedPtr<FString>& Item)
		{
			return (Item.IsValid() && *Item == InName.ToString());
		});

	return (FoundItem != nullptr ? *FoundItem : nullptr);
}

void FNativeLessPulldownStructDetail::OnSourceAssetChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo)
{
	if (!SelectedItem.IsValid() || !PulldownSourceHandle.IsValid())
	{
		return;
	}

	FName NewPulldownSource = **SelectedItem;
	FName OldPulldownSource;
	PulldownSourceHandle->GetValue(OldPulldownSource);
	if (NewPulldownSource != OldPulldownSource)
	{
		PulldownSourceHandle->SetValue(NewPulldownSource);
		RebuildPulldown();
	}
}

FUIAction FNativeLessPulldownStructDetail::CreateCopyAction()
{
	return FUIAction
	(
		FExecuteAction::CreateSP(this, &FNativeLessPulldownStructDetail::OnCopyAction)
	);
}

FUIAction FNativeLessPulldownStructDetail::CreatePasteAction()
{
	return FUIAction
	(
		FExecuteAction::CreateSP(this, & FNativeLessPulldownStructDetail::OnPasteAction)
	);
}

void FNativeLessPulldownStructDetail::OnCopyAction()
{
	if (!SelectedValueHandle.IsValid())
	{
		return;
	}

	FName SelectedValue;
	SelectedValueHandle->GetValue(SelectedValue);

	FPlatformApplicationMisc::ClipboardCopy(*SelectedValue.ToString());
}

void FNativeLessPulldownStructDetail::OnPasteAction()
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
