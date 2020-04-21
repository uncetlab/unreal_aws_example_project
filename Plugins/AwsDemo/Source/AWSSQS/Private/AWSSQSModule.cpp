#include "AWSSQSModule.h"
#include "AWSSQSPrivatePCH.h"
#include "HAL/PlatformProcess.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"


void* FAWSSQSModule::SQSTPModuleDll = nullptr;


TSet<void*> FAWSSQSModule::ValidDllHandles = TSet<void*>();

void FAWSSQSModule::StartupModule(){
#if PLATFORM_WINDOWS && PLATFORM_64BITS
    //If we are on a windows platform we need to Load the DLL's
    UE_LOG(LogTemp, Display, TEXT("Start Loading DLL's"));
    const FString PluginDir = IPluginManager::Get().FindPlugin("AwsDemo")->GetBaseDir();
    

    
    const FString SQSTPModuleName = "aws-cpp-sdk-sqs";
    FString SQSTPModuleDllDir = FPaths::Combine(*PluginDir, TEXT("Source"), TEXT("ThirdParty"), TEXT("SQSTPModule"), TEXT("Win64"));
    const FString SQSTPModulePath = FPaths::Combine(SQSTPModuleDllDir, SQSTPModuleName) + TEXT(".") + FPlatformProcess::GetModuleExtension();
    if (!FAWSSQSModule::LoadDll(SQSTPModulePath, FAWSSQSModule::SQSTPModuleDll, SQSTPModuleName)) {
        FAWSSQSModule::FreeAllDll();
    }
    
#endif
}

void FAWSSQSModule::ShutdownModule(){   
#if PLATFORM_WINDOWS && PLATFORM_64BITS
    FAWSSQSModule::FreeAllDll();
#endif
}

bool FAWSSQSModule::LoadDll(const FString path, void*& dll_ptr, const FString name) {
    //load the passed in dll and if it successeds then add it to the valid set
    UE_LOG(LogTemp, Error, TEXT("Attempting to load DLL %s from %s"), *name, *path);
    dll_ptr = FPlatformProcess::GetDllHandle(*path);

    if (dll_ptr == nullptr) {
        UE_LOG(LogTemp, Error, TEXT("Could not load %s from %s"), *name, *path);
        return false;
    }

    UE_LOG(LogTemp, Display, TEXT("Loaded %s from %s"), *name, *path);
    FAWSSQSModule::ValidDllHandles.Add(dll_ptr);
    return true;
}

void FAWSSQSModule::FreeDll(void*& dll_ptr) {
    //free the dll handle
    if (dll_ptr != nullptr) {
        FPlatformProcess::FreeDllHandle(dll_ptr);
        dll_ptr = nullptr;
    }
}

void FAWSSQSModule::FreeAllDll() {
    //Free all the current valid dll's
    for (auto dll : FAWSSQSModule::ValidDllHandles) {
        FAWSSQSModule::ValidDllHandles.Remove(dll);
        FAWSSQSModule::FreeDll(dll);
    }
}


IMPLEMENT_MODULE(FAWSSQSModule, AWSSQS);