// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UToolMenu;

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
		
	public:
		// Register-Unregister a graph pin context menu extension.
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
		static bool CanSelectedValuePasteAction(TWeakObjectPtr<const UGraphNodeContextMenuContext> Context);
		static bool CanBrowsePulldownContentsAction(TWeakObjectPtr<const UGraphNodeContextMenuContext> Context);
	};
}
