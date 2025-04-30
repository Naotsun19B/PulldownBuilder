// Copyright Epic Games, Inc. All Rights Reserved.

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
	}
}
