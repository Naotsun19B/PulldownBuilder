// Copyright 2021-2023 Naotsun. All Rights Reserved.

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
