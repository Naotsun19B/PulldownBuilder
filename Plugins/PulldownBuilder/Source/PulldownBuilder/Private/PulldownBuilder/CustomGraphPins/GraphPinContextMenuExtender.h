// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UToolMenu;
class UGraphNodeContextMenuContext;

namespace PulldownBuilder
{
	/**
	 * An extender class that adds the functionality used by this plugin to the graph pin's context menu.
	 */
	class PULLDOWNBUILDER_API FGraphPinContextMenuExtender
	{
	public:
		// The name of the section added by the extension.
		static const FName ExtensionSectionName;

		// The event called when the default value of a pin is changed.
		DECLARE_MULTICAST_DELEGATE(FOnPinDefaultValueChanged);
		static FOnPinDefaultValueChanged OnPinDefaultValueChanged;
		
	public:
		// Registers-Unregisters a graph pin context menu extension.
		static void Register();
		static void Unregister();

	private:
		// Returns the name of the context menu extension point for all graph pins.
		static const TArray<FName>& GetAllExtensionPointNames();

		// Called when expanding a graph pin's context menu.
		static void OnExtendMenu(UToolMenu* Menu);

		// Called when a function is invoked from the pull-down struct's context menu.
		static void OnSelectedValueCopyAction(TWeakObjectPtr<const UGraphNodeContextMenuContext> Context);
		static void OnSelectedValuePasteAction(TWeakObjectPtr<const UGraphNodeContextMenuContext> Context);
		static void OnBrowsePulldownContentsAction(TWeakObjectPtr<const UGraphNodeContextMenuContext> Context);

		// Returns whether the function can be called from the pull-down struct's context menu.
		static bool CanSelectedValueCopyAction(TWeakObjectPtr<const UGraphNodeContextMenuContext> Context);
		static bool CanSelectedValuePasteAction(TWeakObjectPtr<const UGraphNodeContextMenuContext> Context);
		static bool CanBrowsePulldownContentsAction(TWeakObjectPtr<const UGraphNodeContextMenuContext> Context);
		
		// Called when the command executes.
		static void HandleOnCommandRun(const FName& CommandName, const FText& CommandLabel);
		
	private:
		// The handle to the event that is called when the command executes.
		static FDelegateHandle CommandRunHandle;
	};
}
