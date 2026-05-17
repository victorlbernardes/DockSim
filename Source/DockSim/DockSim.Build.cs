using UnrealBuildTool;

public class DockSim : ModuleRules
{
	public DockSim(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		bUseUnity = false;

		bLegacyPublicIncludePaths = false;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG" 
		});

		PrivateDependencyModuleNames.AddRange(new string[] { 
			"Slate", "SlateCore" 
		});
	}
}