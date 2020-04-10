using UnrealBuildTool;
using System.IO;

public class AWSBase : ModuleRules{
    public AWSBase(ReadOnlyTargetRules Target) : base(Target){
        PrivatePCHHeaderFile = "Private/AWSBaseModulePrivatePCH.h";

        PublicDependencyModuleNames.AddRange(new string[] { "Engine", "Core", "CoreUObject", "InputCore", "Projects", "BaseTPLibrary",});
        PrivateDependencyModuleNames.AddRange(new string[] { });

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));

        PublicDefinitions.Add("USE_IMPORT_EXPORT");

        if (Target.Platform == UnrealTargetPlatform.Android || Target.Platform == UnrealTargetPlatform.Win64){
           
        }
        else{
        
        }
    }
}