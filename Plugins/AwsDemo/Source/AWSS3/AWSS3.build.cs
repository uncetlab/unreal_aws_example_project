using UnrealBuildTool;
using System.IO;

public class AWSS3 : ModuleRules
{
    public AWSS3(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivatePCHHeaderFile = "Private/AWSS3PrivatePCH.h";

        bEnableExceptions = true;

        PublicDependencyModuleNames.AddRange(new string[] { "Engine", "Core", "CoreUObject", "InputCore", "Projects", "AWSBase", "BaseTPLibrary" ,"S3TPModule", });
        PrivateDependencyModuleNames.AddRange(new string[] { });

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));


        if (Target.Type != TargetRules.TargetType.Server)
        {
            if (Target.Platform == UnrealTargetPlatform.Android || Target.Platform == UnrealTargetPlatform.Win64)
            {
                PublicDefinitions.Add("WITH_s3=1");
            }
            else
            {
                PublicDefinitions.Add("WITH_s3=0");
            }
        }
        else
        {
           PublicDefinitions.Add("WITH_s3=0");
        }
    }
}