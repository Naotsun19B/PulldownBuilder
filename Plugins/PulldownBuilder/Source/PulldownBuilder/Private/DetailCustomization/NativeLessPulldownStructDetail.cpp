// Copyright 2021 Naotsun. All Rights Reserved.

#include "DetailCustomization/NativeLessPulldownStructDetail.h"
#include "Asset/PulldownContents.h"
#include "Utility/PulldownBuilderUtils.h"
#include "Utility/SSearchableTextComboBox.h"
#include "NativeLessPulldownStruct.h"
#include "DetailWidgetRow.h"
#include "PropertyEditorModule.h"
#include "PropertyHandle.h"
#include "IDetailChildrenBuilder.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Modules/ModuleManager.h"

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
	FPulldownStructDetail::CustomizeHeader(InStructPropertyHandle, HeaderRow, StructCustomizationUtils);

	// Scan the properties of the structure for the property handle of FNativeLessPulldownStruct::PulldownContentsName.
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
				if (ChildProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource))
				{
					PulldownSourceHandle = ChildPropertyHandle;
				}
			}
		}
	}

	// Do not register structures other than FNativeLessPulldownStruct in this detail customization.
	check(PulldownSourceHandle.IsValid());
}

void FNativeLessPulldownStructDetail::RefreshPulldownWidget()
{
	check(PulldownSourceHandle);
	
	// Check if the currently set string is included in the constructed list.
	FName CurrentPulldownSource;
	PulldownSourceHandle->GetValue(CurrentPulldownSource);

	TSharedPtr<FString> SelectedItem = FindPulldownContentsNameByName(CurrentPulldownSource);
	if (!SelectedItem.IsValid())
	{
		PulldownSourceHandle->SetValue(NAME_None);
		SelectedItem = FindPulldownContentsNameByName(NAME_None);
	}

	if (PulldownSourceWidget.IsValid())
	{
		PulldownSourceWidget->RefreshOptions();
		PulldownSourceWidget->SetSelectedItem(SelectedItem);
	}

	FPulldownStructDetail::RefreshPulldownWidget();
}

TArray<TSharedPtr<FString>> FNativeLessPulldownStructDetail::GenerateSelectableValues()
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

	FName PulldownSource;
	PulldownSourceHandle->GetValue(PulldownSource);
	if (UPulldownContents* SourceAsset = FPulldownBuilderUtils::FindPulldownContentsByName(PulldownSource))
	{
		return SourceAsset->GetDisplayStrings();
	}

	return FPulldownBuilderUtils::GetEmptyDisplayStrings();
}

void FNativeLessPulldownStructDetail::OnMultipleSelected()
{
	PulldownContentsNames.Reset();
	FPulldownStructDetail::OnMultipleSelected();
}

#if BEFORE_UE_4_24
bool FNativeLessPulldownStructDetail::IsCustomizationTarget(UProperty* InProperty) const
#else
bool FNativeLessPulldownStructDetail::IsCustomizationTarget(FProperty* InProperty) const
#endif
{
	return (
		FPulldownStructDetail::IsCustomizationTarget(InProperty) ||
		InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource)
	);
}

void FNativeLessPulldownStructDetail::AddCustomRowBeforeSelectedValue(IDetailChildrenBuilder& StructBuilder)
{
	StructBuilder.AddCustomRow(FText::FromName(GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource)))
		.CopyAction(CreatePulldownSourceCopyAction())
		.PasteAction(CreatePulldownSourcePasteAction())
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
				SAssignNew(PulldownSourceWidget, SSearchableTextComboBox)
				.OptionsSource(&PulldownContentsNames)
				.OnSelectionChanged(this, &FNativeLessPulldownStructDetail::OnPulldownSourceChanged)
				.OnComboBoxOpening(this, &FNativeLessPulldownStructDetail::RebuildPulldown)
			]
		];
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

void FNativeLessPulldownStructDetail::OnPulldownSourceChanged(TSharedPtr<FString> SelectedItem, ESelectInfo::Type SelectInfo)
{
	check(PulldownSourceHandle && SelectedValueHandle);
	
	if (!SelectedItem.IsValid())
	{
		return;
	}

	FName NewPulldownSource = **SelectedItem;
	FName OldPulldownSource;
	PulldownSourceHandle->GetValue(OldPulldownSource);
	if (NewPulldownSource != OldPulldownSource)
	{
		PulldownSourceHandle->SetValue(NewPulldownSource);

		// Since the base asset of the pull-down menu has changed, set SelectedValue to None.
		SelectedValueHandle->SetValue(FName(NAME_None));
		
		FPulldownStructDetail::RefreshPulldownWidget();
	}
}

FUIAction FNativeLessPulldownStructDetail::CreatePulldownSourceCopyAction()
{
	return FUIAction
	(
		FExecuteAction::CreateSP(this, &FNativeLessPulldownStructDetail::OnPulldownSourceCopyAction)
	);
}

FUIAction FNativeLessPulldownStructDetail::CreatePulldownSourcePasteAction()
{
	return FUIAction
	(
		FExecuteAction::CreateSP(this, & FNativeLessPulldownStructDetail::OnPulldownSourcePasteAction)
	);
}

void FNativeLessPulldownStructDetail::OnPulldownSourceCopyAction()
{
	check(PulldownSourceHandle);

	FName SelectedValue;
	PulldownSourceHandle->GetValue(SelectedValue);

	FPlatformApplicationMisc::ClipboardCopy(*SelectedValue.ToString());
}

void FNativeLessPulldownStructDetail::OnPulldownSourcePasteAction()
{
	check(PulldownSourceWidget);
	
	FName PastedText;
	{
		FString ClipboardString;
		FPlatformApplicationMisc::ClipboardPaste(ClipboardString);
		PastedText = *ClipboardString;
	}

	TSharedPtr<FString> SelectedItem = FindPulldownContentsNameByName(PastedText);
	if (SelectedItem.IsValid())
	{
		PulldownSourceWidget->SetSelectedItem(SelectedItem);
	}
}
