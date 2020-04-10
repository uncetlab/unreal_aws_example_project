//For more information about this file check out https://wiki.unrealengine.com/An_Introduction_to_UE4_Plugins
using System.IO;
using UnrealBuildTool;

public class S3TPModule : ModuleRules{
    public S3TPModule(ReadOnlyTargetRules Target) : base(Target){
        Type = ModuleType.External;

        PublicIncludePaths.Add(ModuleDirectory);

        string ThirdPartyPath = System.IO.Path.Combine(ModuleDirectory, Target.Platform.ToString());

        if (Target.Platform == UnrealTargetPlatform.Android){
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", System.IO.Path.Combine(ModuleDirectory, "S3TPModule_APL.xml"));
            {
                PublicLibraryPaths.Add(System.IO.Path.Combine(ThirdPartyPath, "armeabi-v7a"));
                PublicAdditionalLibraries.Add(System.IO.Path.Combine(ThirdPartyPath, "armeabi-v7a", "libaws-cpp-sdk-s3.so"));
                RuntimeDependencies.Add(System.IO.Path.Combine(ThirdPartyPath, "armeabi-v7a", "libaws-cpp-sdk-s3.so"));
            }
            {
                PublicLibraryPaths.Add(System.IO.Path.Combine(ThirdPartyPath, "arm64-v8a"));
                PublicAdditionalLibraries.Add(System.IO.Path.Combine(ThirdPartyPath, "arm64-v8a", "libaws-cpp-sdk-s3.so"));
                RuntimeDependencies.Add(System.IO.Path.Combine(ThirdPartyPath, "arm64-v8a", "libaws-cpp-sdk-s3.so"));
            }
        }
       
        else if (Target.Platform == UnrealTargetPlatform.Win64){
            PublicLibraryPaths.Add(ThirdPartyPath);
            PublicAdditionalLibraries.Add(System.IO.Path.Combine(ThirdPartyPath, "aws-cpp-sdk-s3.lib"));
            PublicDelayLoadDLLs.Add("aws-cpp-sdk-s3.dll");
            RuntimeDependencies.Add(System.IO.Path.Combine(ThirdPartyPath, "aws-cpp-sdk-s3.dll"));
        }
    }
}