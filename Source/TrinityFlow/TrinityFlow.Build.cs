// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TrinityFlow : ModuleRules
{
	public TrinityFlow(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Slate", "SlateCore", "AIModule", "NavigationSystem" });
	}
}
