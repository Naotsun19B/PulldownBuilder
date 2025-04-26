// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/CustomGraphPins/GraphPinContextMenuExtender.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "ToolMenus.h"
#include "HAL/PlatformApplicationMisc.h"
#include "UObject/UObjectIterator.h"
#include "EdGraphSchema_K2.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "Debugging/SlateDebugging.h"
#include "Editor.h"
#include "GraphEditorActions.h"
#if UE_5_01_OR_LATER
#include "Styling/AppStyle.h"
#else
#include "EditorStyleSet.h"
#endif

#define LOCTEXT_NAMESPACE "GraphPinContextMenuExtender"

namespace PulldownBuilder
{
	const FName FGraphPinContextMenuExtender::ExtensionSectionName = TEXT("PulldownBuilder.GraphPinContextMenuExtender");
	FGraphPinContextMenuExtender::FOnPinDefaultValueChanged FGraphPinContextMenuExtender::OnPinDefaultValueChanged;
	
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

		// Unregister a graph pin context menu extension when before editor close.
		if (IsValid(GEditor))
		{
			GEditor->OnEditorClose().AddStatic(&FGraphPinContextMenuExtender::Unregister);
		}

#if WITH_SLATE_DEBUGGING
		CommandRunHandle = FSlateDebugging::CommandRun.AddStatic(&FGraphPinContextMenuExtender::HandleOnCommandRun);
#endif
	}

	void FGraphPinContextMenuExtender::Unregister()
	{
#if WITH_SLATE_DEBUGGING
		FSlateDebugging::CommandRun.Remove(CommandRunHandle);
#endif
		
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
		
		const TWeakObjectPtr<const UGraphNodeContextMenuContext> WeakContext = Context;
		
		MenuSection.AddMenuEntry(
			TEXT("CopyPinValue"),
			LOCTEXT("CopyPinValueLabel", "Copy"),
			LOCTEXT("CopyPinValueTooltip", "Copies the value selected in the pin's pulldown struct to the clipboard."),
			FSlateIcon(
#if UE_5_01_OR_LATER
				FAppStyle::GetAppStyleSetName(),
#else
				FEditorStyle::GetStyleSetName(),
#endif
				TEXT("GenericCommands.Copy")
			),
			FToolUIActionChoice(
				FUIAction(
					FExecuteAction::CreateStatic(&FGraphPinContextMenuExtender::OnSelectedValueCopyAction, WeakContext),
					FCanExecuteAction::CreateStatic(&FGraphPinContextMenuExtender::CanSelectedValueCopyAction, WeakContext)
				)
			)
		);
		
		MenuSection.AddMenuEntry(
			TEXT("PastePinValue"),
			LOCTEXT("PastePinValueLabel", "Paste"),
			LOCTEXT("PastePinValueTooltip", "Pastes the value from the clipboard into the pin's pulldown struct."),
			FSlateIcon(
#if UE_5_01_OR_LATER
				FAppStyle::GetAppStyleSetName(),
#else
				FEditorStyle::GetStyleSetName(),
#endif
				TEXT("GenericCommands.Paste")
			),
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
			FSlateIcon(
#if UE_5_01_OR_LATER
				FAppStyle::GetAppStyleSetName(),
#else
				FEditorStyle::GetStyleSetName(),
#endif
				TEXT("SystemWideCommands.FindInContentBrowser")
			),
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

		const TSharedPtr<FString> PinValue = FPulldownBuilderUtils::StructStringToMemberValue(
			Pin->GetDefaultAsString(),
			GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue)
		);
		if (PinValue.IsValid())
		{
			FPlatformApplicationMisc::ClipboardCopy(**PinValue);
		}
	}

	void FGraphPinContextMenuExtender::OnSelectedValuePasteAction(TWeakObjectPtr<const UGraphNodeContextMenuContext> Context)
	{
		if (!Context.IsValid())
		{
			return;
		}

		auto* Pin = const_cast<UEdGraphPin*>(Context->Pin);
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
			Schema->TrySetDefaultValue(*Pin, *NewDefaultValue);
		}

		if (UEdGraphNode* Node = Pin->GetOwningNode())
		{
			Node->PinDefaultValueChanged(Pin);

			OnPinDefaultValueChanged.Broadcast();
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

	bool FGraphPinContextMenuExtender::CanSelectedValueCopyAction(TWeakObjectPtr<const UGraphNodeContextMenuContext> Context)
	{
		if (!Context.IsValid())
		{
			return false;
		}
		
		const UEdGraphPin* Pin = Context->Pin;
		if (Pin == nullptr)
		{
			return false;
		}

		return (Pin->LinkedTo.Num() == 0); 
	}

	bool FGraphPinContextMenuExtender::CanSelectedValuePasteAction(TWeakObjectPtr<const UGraphNodeContextMenuContext> Context)
	{
		if (!Context.IsValid())
		{
			return false;
		}

		bool bIsGraphEditable = false;
		if (const UEdGraph* Graph = Context->Graph)
		{
			bIsGraphEditable = Graph->bEditable;
		}

		bool bHasAnyPinConnection = true;
		if (const UEdGraphPin* Pin = Context->Pin)
		{
			bHasAnyPinConnection = (Pin->LinkedTo.Num() != 0);
		}
		
		return (bIsGraphEditable && !bHasAnyPinConnection);
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
	
	void FGraphPinContextMenuExtender::HandleOnCommandRun(const FName& CommandName, const FText& CommandLabel)
	{
		// When you reset a pin to its default value, the pin's appearance will not be reflected if you do not update it.
		const TSharedPtr<FUICommandInfo>& ResetPinToDefaultValue = FGraphEditorCommands::Get().ResetPinToDefaultValue;
		check(ResetPinToDefaultValue.IsValid());

		if (CommandName.IsEqual(ResetPinToDefaultValue->GetCommandName()) && CommandLabel.EqualTo(ResetPinToDefaultValue->GetLabel()))
		{
			check(IsValid(GEditor));

			// Throw an event in the next frame because the default value of the pin must be reset before the update process can take place.
			const TSharedRef<FTimerManager>& TimerManager = GEditor->GetTimerManager();
			TimerManager->SetTimerForNextTick(
				[]()
				{
					OnPinDefaultValueChanged.Broadcast();
				}
			);
		}
	}

	FDelegateHandle FGraphPinContextMenuExtender::CommandRunHandle;
}

#undef LOCTEXT_NAMESPACE
