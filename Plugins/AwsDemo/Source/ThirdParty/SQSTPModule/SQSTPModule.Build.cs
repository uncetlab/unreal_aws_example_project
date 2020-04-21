//For more information about this file check out https://wiki.unrealengine.com/An_Introduction_to_UE4_Plugins
using System.IO;
using UnrealBuildTool;

public class SQSTPModule : ModuleRules{
    public SQSTPModule(ReadOnlyTargetRules Target) : base(Target){
        Type = ModuleType.External;

        PublicIncludePaths.Add(ModuleDirectory);

        string ThirdPartyPath = System.IO.Path.Combine(ModuleDirectory, Target.Platform.ToString());

        if (Target.Platform == UnrealTargetPlatform.Android){
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", System.IO.Path.Combine(ModuleDirectory, "SQSTPModule_APL.xml"));
            {
                PublicLibraryPaths.Add(System.IO.Path.Combine(ThirdPartyPath, "armeabi-v7a"));
                PublicAdditionalLibraries.Add(System.IO.Path.Combine(ThirdPartyPath, "armeabi-v7a", "libaws-cpp-sdk-sqs.so"));
                RuntimeDependencies.Add(System.IO.Path.Combine(ThirdPartyPath, "armeabi-v7a", "libaws-cpp-sdk-sqs.so"));
            }
            {
                PublicLibraryPaths.Add(System.IO.Path.Combine(ThirdPartyPath, "arm64-v8a"));
                PublicAdditionalLibraries.Add(System.IO.Path.Combine(ThirdPartyPath, "arm64-v8a", "libaws-cpp-sdk-sqs.so"));
                RuntimeDependencies.Add(System.IO.Path.Combine(ThirdPartyPath, "arm64-v8a", "libaws-cpp-sdk-sqs.so"));
            }
        }
       
        else if (Target.Platform == UnrealTargetPlatform.Win64){
            PublicLibraryPaths.Add(ThirdPartyPath);
            PublicAdditionalLibraries.Add(System.IO.Path.Combine(ThirdPartyPath, "aws-cpp-sdk-sqs.lib"));
            PublicDelayLoadDLLs.Add("aws-cpp-sdk-sqs.dll");
            RuntimeDependencies.Add(System.IO.Path.Combine(ThirdPartyPath, "aws-cpp-sdk-sqs.dll"));
        }
    }
}