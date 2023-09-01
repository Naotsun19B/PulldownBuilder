// Copyright 2021-2023 Naotsun. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class PulldownStruct : ModuleRules
{
	public PulldownStruct(ReadOnlyTargetRules Target) : base(Target)
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
			}
		);
		
		// Defines a macro that indicates whether this plugin is enabled and the pull-down struct is available.
		// This is useful when you want to embed a pull-down struct without depending on this plugin.
		PublicDefinitions.Add("WITH_PULLDOWN_BUILDER");
	}
}
