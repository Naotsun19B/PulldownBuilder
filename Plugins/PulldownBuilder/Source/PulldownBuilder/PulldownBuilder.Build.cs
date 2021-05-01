﻿// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PulldownBuilder : ModuleRules
{
	public PulldownBuilder(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"PulldownStruct",
			}
			);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"InputCore",
				"Engine",
				"Slate",
				"SlateCore",
				"UnrealEd",
				"AssetTools",
				"EditorStyle",
				"GraphEditor",
				"StructViewer",
				"EditorWidgets",
				"AssetRegistry",
				"BlueprintGraph",
				"PropertyEditor",
				"ApplicationCore",
			}
			);
	}
}
