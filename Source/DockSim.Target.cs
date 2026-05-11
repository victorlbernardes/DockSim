// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class DockSimTarget : TargetRules
{
	public DockSimTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
		ExtraModuleNames.Add("DockSim");

		// Adicione esta linha para isolar o ambiente de compilação
		bOverrideBuildEnvironment = true;
	}
}
