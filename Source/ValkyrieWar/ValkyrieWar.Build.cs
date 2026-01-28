// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ValkyrieWar : ModuleRules
{
	public ValkyrieWar(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { 
            "Core", "CoreUObject", "Engine", "InputCore", 
            "NavigationSystem", "AIModule", "Niagara", "EnhancedInput" });
        PrivateDependencyModuleNames.AddRange(new string[] {
            "GameplayAbilities", "GameplayTags", "GameplayTasks",
            "UMG"
        });
    }
}
