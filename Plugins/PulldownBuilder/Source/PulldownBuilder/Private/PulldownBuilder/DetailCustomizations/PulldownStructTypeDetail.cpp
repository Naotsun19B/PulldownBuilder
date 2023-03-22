// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownBuilder/DetailCustomizations/PulldownStructTypeDetail.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownBuilder/Types/PulldownStructType.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "StructViewerModule.h"
#include "StructViewerFilter.h"
#include "DetailWidgetRow.h"
#include "PropertyEditorModule.h"
#include "PropertyHandle.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#if UE_5_01_OR_LATER
#include "Styling/AppStyle.h"
#else
#include "EditorStyleSet.h"
#endif

namespace PulldownBuilder
{
	namespace StructFilter
	{
		// Filter class for displaying only structs that inherit from FPulldownStructBase in the struct picker.
		class FPulldownStructFilter : public IStructViewerFilter
		{
		public:
			// IStructViewerFilter interface.
			virtual bool IsStructAllowed(
				const FStructViewerInitializationOptions& InInitOptions,
				const UScriptStruct* InStruct,
				TSharedRef<FStructViewerFilterFuncs> InFilterFuncs
			) override
			{
				bool bIsStructAllowed = false;
				if (FPulldownBuilderUtils::IsPulldownStruct(InStruct))
				{
					bIsStructAllowed = !FPulldownBuilderUtils::IsRegisteredPulldownStruct(InStruct);
				}
			
				return bIsStructAllowed;
			}
			virtual bool IsUnloadedStructAllowed(
				const FStructViewerInitializationOptions& InInitOptions,
#if UE_5_01_OR_LATER
				const FSoftObjectPath& InStructPath,
#else
				const FName InStructPath,
#endif
				TSharedRef<FStructViewerFilterFuncs> InFilterFuncs
			) override
			{
				return false;
			}
			// End of IStructViewerFilter interface.
		};
	}

	void FPulldownStructTypeDetail::Register()
	{
		CachedPropertyTypeName = GetNameSafe(FPulldownStructType::StaticStruct());
		
		auto& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		PropertyEditorModule.RegisterCustomPropertyTypeLayout(
			*CachedPropertyTypeName,
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FPulldownStructTypeDetail::MakeInstance)
		);
	}

	void FPulldownStructTypeDetail::Unregister()
	{
		auto& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout(
			*CachedPropertyTypeName
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
		check(SelectedStructHandle.IsValid() && StructPickerAnchor.IsValid());
	
		SelectedStructHandle->SetValue(SelectedStruct);
		StructPickerAnchor->SetIsOpen(false);
	}

	FText FPulldownStructTypeDetail::OnGetComboTextValue() const
	{
		check(SelectedStructHandle.IsValid());
	
		UObject* Value;
		SelectedStructHandle->GetValue(Value);
		if (const auto* Struct = Cast<UScriptStruct>(Value))
		{
			return FText::AsCultureInvariant(Struct->GetName());
		}

		return NSLOCTEXT("PulldownStructTypeDetail", "NoSelectedStruct", "None");
	}

	TSharedRef<SWidget> FPulldownStructTypeDetail::GenerateStructPicker()
	{
		FStructViewerModule& StructViewerModule = FModuleManager::LoadModuleChecked<FStructViewerModule>("StructViewer");

		FStructViewerInitializationOptions Options;
		Options.StructFilter = MakeShared<StructFilter::FPulldownStructFilter>();
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
					.BorderImage(
#if UE_5_01_OR_LATER
					FAppStyle
#else
					FEditorStyle
#endif
						::GetBrush(TEXT("ToolPanel.GroupBorder"))
					)
					[
						StructViewerModule.CreateStructViewer(Options, FOnStructPicked::CreateSP(this, &FPulldownStructTypeDetail::OnPickedStruct))
					]
				]
			];
	}

	FString FPulldownStructTypeDetail::CachedPropertyTypeName;
}
