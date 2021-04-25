// Copyright 2021 Naotsun. All Rights Reserved.

#include "DetailCustomization/PreviewPulldownStructDetail.h"
#include "PulldownBuilderGlobals.h"
#include "Asset/PulldownContents.h"
#include "Common/PreviewPulldownStruct.h"
#include "PulldownStructBase.h"
#include "DetailWidgetRow.h"
#include "PropertyEditorModule.h"
#include "PropertyHandle.h"
#include "IDetailChildrenBuilder.h"
#include "Widgets/Input/STextComboBox.h"
#include "HAL/PlatformApplicationMisc.h"

void FPreviewPulldownStructDetail::Register()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout(
        FPreviewPulldownStruct::StaticStruct()->GetFName(),
        FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FPreviewPulldownStructDetail::MakeInstance)
    );
}

void FPreviewPulldownStructDetail::Unregister()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomPropertyTypeLayout(
		FPreviewPulldownStruct::StaticStruct()->GetFName()
    );
}

TSharedRef<IPropertyTypeCustomization> FPreviewPulldownStructDetail::MakeInstance()
{
	return MakeShared<FPreviewPulldownStructDetail>();
}

void FPreviewPulldownStructDetail::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
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

	HeaderRow.NameContent()
    [
        StructPropertyHandle->CreatePropertyNameWidget()
    ]
	.ValueContent()
	.MinDesiredWidth(500.f)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		[
			SAssignNew(PulldownWidget, STextComboBox)
			.OptionsSource(&DisplayStrings)
			.OnSelectionChanged(this, &FPreviewPulldownStructDetail::OnStateValueChanged)
			.OnComboBoxOpening(this, & FPreviewPulldownStructDetail::RebuildPulldown)
		]
	];

	RebuildPulldown();
}

void FPreviewPulldownStructDetail::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}

void FPreviewPulldownStructDetail::RebuildPulldown()
{
	if (StructPropertyHandle == nullptr ||
        !StructPropertyHandle->IsValidHandle() ||
        !SelectedValueHandle.IsValid()
    )
	{
		return;
	}

	// Get the list of strings to display from Pulldown Contents that owns this structure.
	TArray<UObject*> OuterObjects;
	StructPropertyHandle->GetOuterObjects(OuterObjects);
	for (const auto& OuterObject : OuterObjects)
	{
		if (auto* PulldownContents = Cast<UPulldownContents>(OuterObject))
		{
			DisplayStrings = PulldownContents->GetDisplayStrings();
			break;
		}
	}

	FPulldownStructDetailBase::RebuildPulldown();
}
