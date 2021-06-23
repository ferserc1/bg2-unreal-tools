// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Texture2D.h"
#include "ImageCache.generated.h"

/**
 * 
 */
UCLASS()
class UImageCache : public UObject
{
	GENERATED_BODY()

public:

    bool FindTexture(const FString& imagePath);
    UTexture2D* GetTexture(const FString& imagePath);
    void AddTexture(const FString& imagePath, UTexture2D* Texture);

private:
    UPROPERTY(Transient)
    mutable TMap<FString,UTexture2D*> TextureCacheMap;
};
