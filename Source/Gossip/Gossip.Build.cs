// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Gossip : ModuleRules
{
	public Gossip(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
