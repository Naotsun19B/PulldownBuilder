// Copyright 2021 Naotsun. All Rights Reserved.

#include "DetailCustomization/PulldownStructTypeDetail.h"
#include "Asset/PulldownContents.h"
#include "Struct/PulldownStructType.h"
#include "Utility/PulldownBuilderUtils.h"
#include "StructViewerModule.h"
#include "StructViewerFilter.h"
#include "DetailWidgetRow.h"
#include "PropertyEditorModule.h"
#include "PropertyHandle.h"

namespace PulldownStructTypeDetailInternal
{
	// Filter class for displaying only structs that inherit from FPulldownStructBase in the struct picker.
	class FPulldownStructFilter : public IStructViewerFilter
	{
	public:
		// IStructViewerFilter interface.
		virtual bool IsStructAllowed(const FStructViewerInitializationOptions& InInitOptions, const UScriptStruct* InStruct, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs) override
		{
			bool bIsStructAllowed = false;
			if (FPulldownBuilderUtils::IsPulldownStruct(InStruct))
			{
				bIsStructAllowed = !FPulldownBuilderUtils::IsRegisteredPulldownStruct(InStruct);
			}
			
			return bIsStructAllowed;
		}
		
		virtual bool IsUnloadedStructAllowed(const FStructViewerInitializationOptions& InInitOptions, const FName InStructPath, TSharedRef<FStructViewerFilterFuncs> InFilterFuncs) override
		{
			return false;
		}
		// End of IStructViewerFilter interface.
	};
}

void FPulldownStructTypeDetail::Register()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout(
		FPulldownStructType::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FPulldownStructTypeDetail::MakeInstance)
	);
}

void FPulldownStructTypeDetail::Unregister()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomPropertyTypeLayout(
		FPulldownStructType::StaticStruct()->GetFName()
	);
}

TSharedRef<IPropertyTypeCustomization> FPulldownStructTypeDetail::MakeInstance()
{
	return MakeShared<FPulldownStructTypeDetail>();
}

void FPulldownStructTypeDetail::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	uint32 NumChildren;
	InStructPropertyHandle->GetNumChildren(NumChildren);

	for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
	{
		const TSharedPtr<IPropertyHandle> ChildHandle = InStructPropertyHandle->GetChildHandle(ChildIndex);
		if (ChildHandle->GetProperty()->GetFName() == GET_MEMBER_NAME_CHECKED(FPulldownStructType, SelectedStruct))
		{
			SelectedStructHandle = ChildHandle;
		}
	}

	HeaderRow
		.NameContent()
		[
			InStructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(200)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(StructPickerAnchor, SComboButton)
				.ContentPadding(FMargin(2, 2, 2, 1))
				.MenuPlacement(MenuPlacement_BelowAnchor)
				.ButtonContent()
				[
					SNew(STextBlock)
					.Text(this, &FPulldownStructTypeDetail::OnGetComboTextValue)
				]
				.OnGetMenuContent(this, &FPulldownStructTypeDetail::GenerateStructPicker)
			]
		];
}

void FPulldownStructTypeDetail::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}

void FPulldownStructTypeDetail::OnPickedStruct(const UScriptStruct* SelectedStruct)
{
	check(SelectedStructHandle && StructPickerAnchor);
	
	SelectedStructHandle->SetValue(SelectedStruct);
	StructPickerAnchor->SetIsOpen(false);
}

FText FPulldownStructTypeDetail::OnGetComboTextValue() const
{
	check(SelectedStructHandle);
	
	UObject* Value;
	SelectedStructHandle->GetValue(Value);
	if (auto* Struct = Cast<UScriptStruct>(Value))
	{
		return FText::AsCultureInvariant(Struct->GetName());
	}

	return NSLOCTEXT("PulldownStructTypeDetail", "NoSelectedStruct", "None");
}

TSharedRef<SWidget> FPulldownStructTypeDetail::GenerateStructPicker()
{
	FStructViewerModule& StructViewerModule = FModuleManager::LoadModuleChecked<FStructViewerModule>("StructViewer");

	FStructViewerInitializationOptions Options;
	Options.StructFilter = MakeShared<PulldownStructTypeDetailInternal::FPulldownStructFilter>();
	Options.Mode = EStructViewerMode::StructPicker;
	Options.bShowNoneOption = true;

	return
		SNew(SBox)
		.WidthOverride(330)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.MaxHeight(500)
			[
				SNew(SBorder)
				.Padding(4)
				.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
				[
					StructViewerModule.CreateStructViewer(Options, FOnStructPicked::CreateSP(this, &FPulldownStructTypeDetail::OnPickedStruct))
				]
			]
		];
}
