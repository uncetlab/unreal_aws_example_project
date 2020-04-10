#pragma once 

#include "Modules/ModuleManager.h"
#include "GenericPlatform/GenericPlatformProcess.h"

class FAWSS3Module : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	static TSet<void*> ValidDllHandles;
	
    static void* S3TPModuleDll;
    

	bool LoadDll(const FString path, void*& dll_ptr, const FString name);

	void FreeDll(void*& dll_ptr);

	void FreeAllDll();

};