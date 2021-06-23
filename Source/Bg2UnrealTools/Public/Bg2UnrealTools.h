#pragma once

#include "ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(Bg2Tools, Display, All);

class Bg2UnrealToolsImpl : public IModuleInterface
{
public:
    void StartupModule();
    void ShutdownModule();
};
