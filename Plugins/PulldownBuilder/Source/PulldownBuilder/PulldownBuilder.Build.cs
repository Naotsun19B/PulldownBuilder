﻿// Copyright 2021-2024 Naotsun. All Rights Reserved.

using UnrealBuildTool;

public class PulldownBuilder : ModuleRules
{
	public PulldownBuilder(ReadOnlyTargetRules Target) : base(Target)
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
				"InputCore",
				"Engine",
				"UnrealEd",
				"Slate",
				"SlateCore",
				"EditorStyle",
				"MessageLog",
				"Projects",
				"ToolMenus",
				"AssetTools",
				"GraphEditor",
				"StructViewer",
				"AssetRegistry",
				"BlueprintGraph",
				"PropertyEditor",
				"ApplicationCore",
				"Json",
				"JsonUtilities",

				"PulldownStruct",
			}
		);
	}
}
