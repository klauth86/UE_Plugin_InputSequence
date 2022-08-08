// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class InputSeqProj_5_00_03Target : TargetRules
{
	public InputSeqProj_5_00_03Target( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "InputSeqProj_5_00_03" } );
	}
}
