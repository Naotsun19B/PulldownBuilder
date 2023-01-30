// Copyright 2021-2023 Naotsun. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class PulldownStruct : ModuleRules
{
	public PulldownStruct(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);
				
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
			}
		);
		
		// To use version macros.
		PublicIncludePaths.AddRange(
			new string[]
			{
				Path.Combine(EngineDirectory, "Source", "Runtime", "Launch", "Resources"),
			}
		);
	}
}
