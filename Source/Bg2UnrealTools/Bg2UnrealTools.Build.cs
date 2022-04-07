using UnrealBuildTool;
using System.IO;

public class Bg2UnrealTools : ModuleRules
{
    public Bg2UnrealTools(ReadOnlyTargetRules Target) :base(Target)
    {
      
        PrivatePCHHeaderFile = "Public/Bg2UnrealToolsPrivatePCH.h";

        PrivateIncludePaths.AddRange(new string[] { "Bg2UnrealTools/Private" });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "SlateCore", "Slate",  "Json", "JsonUtilities", "ProceduralMeshComponent" });

        PrivateDependencyModuleNames.AddRange(new string[] { "ImageWrapper", "RenderCore", "RHI" });
    }
}
