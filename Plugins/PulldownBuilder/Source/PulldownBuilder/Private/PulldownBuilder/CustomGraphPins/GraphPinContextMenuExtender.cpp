// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownBuilder/CustomGraphPins/GraphPinContextMenuExtender.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "ToolMenus.h"
#include "Styling/AppStyle.h"
#include "HAL/PlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "GraphPinContextMenuExtender"

namespace PulldownBuilder
{
	const FName FGraphPinContextMenuExtender::ExtensionSectionName = TEXT("PulldownBuilder.GraphPinContextMenuExtender");
	
	void FGraphPinContextMenuExtender::Register()
	{
		auto* ToolMenus = UToolMenus::Get();
		check(IsValid(ToolMenus));

		const TArray<FName>& ExtensionPointNames = GetAllExtensionPointNames();
		for (const auto& ExtensionPointName : ExtensionPointNames)
		{
			UToolMenu* Menu = ToolMenus->ExtendMenu(ExtensionPointName);
			if (!IsValid(Menu))
			{
				continue;
			}

			Menu->AddDynamicSection(
				ExtensionSectionName,
				FNewSectionConstructChoice(
					FNewToolMenuDelegate::CreateStatic(&FGraphPinContextMenuExtender::OnExtendMenu)
				)
			);
		}
	}

	void FGraphPinContextMenuExtender::Unregister()
	{
		auto* ToolMenus = UToolMenus::Get();
		check(IsValid(ToolMenus));

		const TArray<FName>& ExtensionPointNames = GetAllExtensionPointNames();
		for (const auto& ExtensionPointName : ExtensionPointNames)
		{
			UToolMenu* Menu = ToolMenus->FindMenu(ExtensionPointName);
			if (!IsValid(Menu))
			{
				continue;
			}

			Menu->RemoveSection(ExtensionSectionName);
		}
	}

	const TArray<FName>& FGraphPinContextMenuExtender::GetAllExtensionPointNames()
	{
		static TArray<FName> ExtensionPointNames;
		
		TArray<const UClass*> EdGraphSchemaClasses;
		for (const auto* Class : TObjectRange<UClass>())
		{
			if (!IsValid(Class))
			{
				continue;
			}

			if (!Class->IsChildOf<UEdGraphSchema>())
			{
				continue;
			}
			
			EdGraphSchemaClasses.Add(Class);
		}
		
		if (ExtensionPointNames.Num() != EdGraphSchemaClasses.Num())
		{
			for (const auto* EdGraphSchemaClass : EdGraphSchemaClasses)
			{
				if (!IsValid(EdGraphSchemaClass))
				{
					continue;
				}

				const auto* EdGraphSchema = Cast<UEdGraphSchema>(EdGraphSchemaClass->GetDefaultObject());
				if (!IsValid(EdGraphSchema))
				{
					continue;
				}
				
				const FName& ExtensionPointName = EdGraphSchema->GetContextMenuName();
				if (!ExtensionPointNames.Contains(ExtensionPointName))
				{
					ExtensionPointNames.Add(ExtensionPointName);
				}
			}
		}
		
		return ExtensionPointNames;
	}

	void FGraphPinContextMenuExtender::OnExtendMenu(UToolMenu* Menu)
	{
		if (!IsValid(Menu))
		{
			return;
		}

		const auto* Context = Menu->FindContext<UGraphNodeContextMenuContext>();
		if (!IsValid(Context))
		{
			return;
		}

		const UEdGraphPin* Pin = Context->Pin;
		if (Pin == nullptr)
		{
			return;
		}

		if (Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Struct)
		{
			return;
		}

		const auto* Struct = Cast<UScriptStruct>(Pin->PinType.PinSubCategoryObject);
		if (!IsValid(Struct))
		{
			return;
		}

		if (!FPulldownBuilderUtils::IsPulldownStruct(Struct, false))
		{
			return;
		}

		FToolMenuSection& MenuSection = Menu->AddSection(
			ExtensionSectionName,
			LOCTEXT("SectionName", "Pulldown Struct")
		);
		
		const TWeakObjectPtr<const UGraphNodeContextMenuContext> WeakContext(Context);
		
		MenuSection.AddMenuEntry(
			TEXT("CopyPinValue"),
			LOCTEXT("CopyPinValueLabel", "Copy"),
			LOCTEXT("CopyPinValueTooltip", "Copies the value selected in the pin's pulldown struct to the clipboard."),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), TEXT("GenericCommands.Copy")),
			FToolUIActionChoice(
				FUIAction(
					FExecuteAction::CreateStatic(&FGraphPinContextMenuExtender::OnSelectedValueCopyAction, WeakContext)
				)
			)
		);

		MenuSection.AddMenuEntry(
			TEXT("PastePinValue"),
			LOCTEXT("PastePinValueLabel", "Paste"),
			LOCTEXT("PastePinValueTooltip", "Pastes the value from the clipboard into the pin's pulldown struct."),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), TEXT("GenericCommands.Paste")),
			FToolUIActionChoice(
				FUIAction(
					FExecuteAction::CreateStatic(&FGraphPinContextMenuExtender::OnSelectedValueCopyAction, WeakContext)
				)
			)
		);

		MenuSection.AddMenuEntry(
			TEXT("PastePinValue"),
			LOCTEXT("PastePinValueLabel", "Paste"),
			LOCTEXT("PastePinValueTooltip", "Pastes the value from the clipboard into the pin's pulldown struct."),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), TEXT("GenericCommands.Paste")),
			FToolUIActionChoice(
				FUIAction(
					FExecuteAction::CreateStatic(&FGraphPinContextMenuExtender::OnSelectedValuePasteAction, WeakContext),
					FCanExecuteAction::CreateStatic(&FGraphPinContextMenuExtender::CanSelectedValuePasteAction, WeakContext)
				)
			)
		);

		MenuSection.AddMenuEntry(
			TEXT("OpenSourceAsset"),
			LOCTEXT("OpenSourceAssetLabel", "Open Source Asset"),
			LOCTEXT("OpenSourceAssetTooltip", "Open the underlying pulldown contents asset for the pin's pulldown struct."),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), TEXT("SystemWideCommands.FindInContentBrowser")),
			FToolUIActionChoice(
				FUIAction(
					FExecuteAction::CreateStatic(&FGraphPinContextMenuExtender::OnBrowsePulldownContentsAction, WeakContext),
					FCanExecuteAction::CreateStatic(&FGraphPinContextMenuExtender::CanBrowsePulldownContentsAction, WeakContext)
				)
			)
		);
	}

	void FGraphPinContextMenuExtender::OnSelectedValueCopyAction(TWeakObjectPtr<const UGraphNodeContextMenuContext> Context)
	{
		if (!Context.IsValid())
		{
			return;
		}

		const UEdGraphPin* Pin = Context->Pin;
		if (Pin == nullptr)
		{
			return;
		}

		const TSharedPtr<FName> PinValue = FPulldownBuilderUtils::StructStringToMemberValue(
			Pin->GetDefaultAsString(),
			GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue)
		);
		if (PinValue.IsValid())
		{
			FPlatformApplicationMisc::ClipboardCopy(*PinValue->ToString());
		}
	}

	void FGraphPinContextMenuExtender::OnSelectedValuePasteAction(TWeakObjectPtr<const UGraphNodeContextMenuContext> Context)
	{
		if (!Context.IsValid())
		{
			return;
		}

		const UEdGraphPin* Pin = Context->Pin;
		if (Pin == nullptr)
		{
			return;
		}

		FString ClipboardString;
		FPlatformApplicationMisc::ClipboardPaste(ClipboardString);

		const TSharedPtr<FString> NewDefaultValue = FPulldownBuilderUtils::MemberValueToStructString(
			Pin->GetDefaultAsString(),
			GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue),
			*ClipboardString
		);
		if (NewDefaultValue.IsValid())
		{
			const UEdGraphSchema* Schema = Pin->GetSchema();
			check(IsValid(Schema));
			Schema->TrySetDefaultValue(const_cast<UEdGraphPin&>(*Pin), *NewDefaultValue);
		}
	}

	void FGraphPinContextMenuExtender::OnBrowsePulldownContentsAction(TWeakObjectPtr<const UGraphNodeContextMenuContext> Context)
	{
		const UEdGraphPin* Pin = Context->Pin;
		if (Pin == nullptr)
		{
			return;
		}

		if (Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Struct)
		{
			return;
		}

		const auto* Struct = Cast<UScriptStruct>(Pin->PinType.PinSubCategoryObject);
		if (!IsValid(Struct))
		{
			return;
		}
		
		if (UPulldownContents* PulldownContents = FPulldownBuilderUtils::FindPulldownContentsByStruct(Struct))
    	{
    		FPulldownBuilderUtils::OpenPulldownContents(PulldownContents);
    	}
	}

	bool FGraphPinContextMenuExtender::CanSelectedValuePasteAction(TWeakObjectPtr<const UGraphNodeContextMenuContext> Context)
	{
		if (!Context.IsValid())
		{
			return false;
		}

		const UEdGraph* Graph = Context->Graph;
		if (!IsValid(Graph))
		{
			return false;
		}

		return Graph->bEditable;
	}

	bool FGraphPinContextMenuExtender::CanBrowsePulldownContentsAction(TWeakObjectPtr<const UGraphNodeContextMenuContext> Context)
	{
		const UEdGraphPin* Pin = Context->Pin;
		if (Pin == nullptr)
		{
			return false;
		}

		if (Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Struct)
		{
			return false;
		}

		const auto* Struct = Cast<UScriptStruct>(Pin->PinType.PinSubCategoryObject);
		if (!IsValid(Struct))
		{
			return false;
		}

		return IsValid(FPulldownBuilderUtils::FindPulldownContentsByStruct(Struct));
	}
}

#undef LOCTEXT_NAMESPACE
