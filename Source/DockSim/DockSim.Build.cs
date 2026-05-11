using UnrealBuildTool;

public class DockSim : ModuleRules
{
	public DockSim(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Desativa o Unity Build para este módulo específico. 
		// Isso reduz a quantidade de arquivos processados em lote que podem inflar o ambiente.
		bUseUnity = false;

		// Força a inclusão apenas do necessário
		bLegacyPublicIncludePaths = false;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG" 
		});

		PrivateDependencyModuleNames.AddRange(new string[] { 
			"Slate", "SlateCore" 
		});
	}
}