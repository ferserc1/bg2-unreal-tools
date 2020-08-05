#pragma once

#include "ModuleManager.h"
#include "ImageCache.h"
#include "Engine/World.h"

DECLARE_LOG_CATEGORY_EXTERN(Bg2Tools, Display, All);

class BG2UNREALTOOLS_API Bg2UnrealToolsImpl : public IModuleInterface
{
public:
    void StartupModule();
    void ShutdownModule();

    UImageCache* GetImageCache(UObject * InitialOuterObject);
    void ClearImageCache();

protected:
    UPROPERTY()
    UImageCache* mImageCache = nullptr;

    UObject* mOuterObject = nullptr;

};
