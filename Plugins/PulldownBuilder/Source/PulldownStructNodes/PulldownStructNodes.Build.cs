// Copyright 2021-2025 Naotsun. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class PulldownStructNodes : ModuleRules
{
	public PulldownStructNodes(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
#if UE_5_2_OR_LATER
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
#endif
		
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
				"SlateCore",
				"BlueprintGraph",
				"Kismet",
				"KismetCompiler",
				
				"PulldownStruct",
				"PulldownBuilder",
			}
		);
	}
}
