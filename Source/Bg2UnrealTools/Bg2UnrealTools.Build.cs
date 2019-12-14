using UnrealBuildTool;
using System.IO;

public class Bg2UnrealTools : ModuleRules
{
    public Bg2UnrealTools(ReadOnlyTargetRules Target) :base(Target)
    {
        PrivateIncludePaths.AddRange(new string[] { "Bg2UnrealTools/Private" });
        PublicIncludePaths.AddRange(new string[] { "Bg2UnrealTools/Public" });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "SlateCore", "Json", "JsonUtilities", "ProceduralMeshComponent" });
    }
}
