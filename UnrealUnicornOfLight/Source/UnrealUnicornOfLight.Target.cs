// Copyright SomethingNotRandom. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UnrealUnicornOfLightTarget : TargetRules
{
	public UnrealUnicornOfLightTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "UnrealUnicornOfLight" } );
	}
}
