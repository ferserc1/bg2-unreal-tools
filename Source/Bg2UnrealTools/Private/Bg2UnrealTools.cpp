
#include "Bg2UnrealToolsPrivatePCH.h"

#include "Bg2UnrealTools.h"

#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"


DEFINE_LOG_CATEGORY(Bg2Tools);

void Bg2UnrealToolsImpl::StartupModule()
{

}

void Bg2UnrealToolsImpl::ShutdownModule()
{
}

UImageCache * Bg2UnrealToolsImpl::GetImageCache(UObject* InitialOuterObject)
{
	if (mImageCache == nullptr || InitialOuterObject != mOuterObject)
	{
		mOuterObject = InitialOuterObject;
		mImageCache = NewObject<UImageCache>(InitialOuterObject);
	}
	return mImageCache;
}

void Bg2UnrealToolsImpl::ClearImageCache()
{
	mImageCache = nullptr;
}

IMPLEMENT_MODULE(Bg2UnrealToolsImpl, Bg2UnrealTools)
