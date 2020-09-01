#pragma once

#include "Modules/ModuleManager.h"
#include "ImageCache.h"
#include "Engine/World.h"

#define BG2_ON_SCREEN_MESSAGES 1
#define BG2_LOG(t, c, msg) if (BG2_ON_SCREEN_MESSAGES==1) { GEngine->AddOnScreenDebugMessage(-1, t, c, msg); UE_LOG(LogTemp, Warning, TEXT("%s"), *msg); }

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
