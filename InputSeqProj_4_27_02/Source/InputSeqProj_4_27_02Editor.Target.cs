// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class InputSeqProj_4_27_02EditorTarget : TargetRules
{
	public InputSeqProj_4_27_02EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "InputSeqProj_4_27_02" } );
	}
}
