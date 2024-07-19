// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class P1 : ModuleRules
{
	public P1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "Networking", "Sockets", "InputCore", "Niagara", "AIModule","EnhancedInput" });

        PrivateDependencyModuleNames.AddRange(new string[] { "ProtobufCore" });

        PrivateIncludePaths.AddRange(new string[]
        {
            "P1/Network/",
            "P1/",
        });
    }
}
