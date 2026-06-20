// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class PulldownBuilderTestEditor : ModuleRules
{
	public PulldownBuilderTestEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
#if UE_5_2_OR_LATER
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
#endif

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "PulldownBuilderTest", "PulldownStruct", "PulldownBuilder" });

		// Expose the PulldownBuilder plugin's Private/ directory so AutomationTest code can subclass
		// editor-only utility types (e.g. UDataTableUpdater) declared there to write regression tests.
		// C++ access modifiers still apply -- only protected / public APIs become reachable.
		PrivateIncludePaths.Add(
			Path.Combine(
				ModuleDirectory,
				"..", "..",
				"Plugins", "PulldownBuilder", "Source", "PulldownBuilder", "Private"
			)
		);
	}
}
