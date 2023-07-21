// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PulldownBuilderTestEditorTarget : TargetRules
{
	public PulldownBuilderTestEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
#if UE_4_24_OR_LATER
		DefaultBuildSettings = BuildSettingsVersion.V2;
#endif
#if UE_5_2_OR_LATER
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
#endif
		ExtraModuleNames.AddRange( new string[] { "PulldownBuilderTestEditor" } );
	}
}
