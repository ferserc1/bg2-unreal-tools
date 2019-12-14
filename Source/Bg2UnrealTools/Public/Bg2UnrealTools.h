#pragma once

#include "ModuleManager.h"

class Bg2UnrealToolsImpl : public IModuleInterface
{
public:
    void StartupModule();
    void ShutdownModule();
};
