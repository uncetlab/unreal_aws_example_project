#include "AWSS3Module.h"
#include "AWSS3PrivatePCH.h"
#include "HAL/PlatformProcess.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"


void* FAWSS3Module::S3TPModuleDll = nullptr;


TSet<void*> FAWSS3Module::ValidDllHandles = TSet<void*>();

void FAWSS3Module::StartupModule(){
#if PLATFORM_WINDOWS && PLATFORM_64BITS
    //If we are on a windows platform we need to Load the DLL's
    UE_LOG(LogTemp, Display, TEXT("Start Loading DLL's"));
    const FString PluginDir = IPluginManager::Get().FindPlugin("AwsDemo")->GetBaseDir();
    

    
    const FString S3TPModuleName = "aws-cpp-sdk-s3";
    FString S3TPModuleDllDir = FPaths::Combine(*PluginDir, TEXT("Source"), TEXT("ThirdParty"), TEXT("S3TPModule"), TEXT("Win64"));
    const FString S3TPModulePath = FPaths::Combine(S3TPModuleDllDir, S3TPModuleName) + TEXT(".") + FPlatformProcess::GetModuleExtension();
    if (!FAWSS3Module::LoadDll(S3TPModulePath, FAWSS3Module::S3TPModuleDll, S3TPModuleName)) {
        FAWSS3Module::FreeAllDll();
    }
    
#endif
}

void FAWSS3Module::ShutdownModule(){   
#if PLATFORM_WINDOWS && PLATFORM_64BITS
    FAWSS3Module::FreeAllDll();
#endif
}

bool FAWSS3Module::LoadDll(const FString path, void*& dll_ptr, const FString name) {
    //load the passed in dll and if it successeds then add it to the valid set
    UE_LOG(LogTemp, Error, TEXT("Attempting to load DLL %s from %s"), *name, *path);
    dll_ptr = FPlatformProcess::GetDllHandle(*path);

    if (dll_ptr == nullptr) {
        UE_LOG(LogTemp, Error, TEXT("Could not load %s from %s"), *name, *path);
        return false;
    }

    UE_LOG(LogTemp, Display, TEXT("Loaded %s from %s"), *name, *path);
    FAWSS3Module::ValidDllHandles.Add(dll_ptr);
    return true;
}

void FAWSS3Module::FreeDll(void*& dll_ptr) {
    //free the dll handle
    if (dll_ptr != nullptr) {
        FPlatformProcess::FreeDllHandle(dll_ptr);
        dll_ptr = nullptr;
    }
}

void FAWSS3Module::FreeAllDll() {
    //Free all the current valid dll's
    for (auto dll : FAWSS3Module::ValidDllHandles) {
        FAWSS3Module::ValidDllHandles.Remove(dll);
        FAWSS3Module::FreeDll(dll);
    }
}


IMPLEMENT_MODULE(FAWSS3Module, AWSS3);