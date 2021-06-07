// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Spotify : ModuleRules
{
	public Spotify(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"Http",
			"Networking",
			"Sockets",
			"Json",
			"DeveloperSettings"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});
	}
}
