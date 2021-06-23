#pragma once

#include "PixelFormat.h"
#include "ImageLoader.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class UImageLoader : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = ImageLoader, meta = (HidePin = "Outer", DefaultToSelf = "Outer"))
	static UTexture2D * LoadImageFromDisk(UObject * Outer, const FString & ImagePath);

private:
	static UTexture2D * CreateTexture(UObject * Outer, const TArray<uint8> & PixelData, int32 InSizeX, int32 InSizeY, EPixelFormat PixelFormat = EPixelFormat::PF_B8G8R8A8, FName BaseName = NAME_None);
};