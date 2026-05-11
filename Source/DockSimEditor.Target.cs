// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class DockSimEditorTarget : TargetRules
{
	public DockSimEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
		ExtraModuleNames.Add("DockSim");

		// ESSA LINHA É A SOLUÇÃO:
		// Força o Unreal a ignorar o PATH do Windows e criar um ambiente limpo.
		bOverrideBuildEnvironment = true;
		bUseUnityBuild = false;
	}
}
