// Copyright 2021-2022 Naotsun. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class PulldownStructNodes : ModuleRules
{
	public PulldownStructNodes(ReadOnlyTargetRules Target) : base(Target)
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
				"UnrealEd",
				"BlueprintGraph",
				"Kismet",
				"KismetCompiler",
				
				"PulldownStruct",
				"PulldownBuilder",
			}
		);
	}
}
