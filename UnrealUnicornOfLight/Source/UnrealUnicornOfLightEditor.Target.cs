// Copyright SomethingNotRandom. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UnrealUnicornOfLightEditorTarget : TargetRules
{
	public UnrealUnicornOfLightEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "UnrealUnicornOfLight" } );
	}
}
