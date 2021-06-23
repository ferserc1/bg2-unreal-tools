// Fill out your copyright notice in the Description page of Project Settings.


#include "ImageCache.h"


bool UImageCache::FindTexture(const FString& imagePath)
{
	return TextureCacheMap.Contains(imagePath);
}

UTexture2D* UImageCache::GetTexture(const FString& imagePath)
{
	if (FindTexture(imagePath))
	{
		return TextureCacheMap[imagePath];
	}
	else
	{
		return nullptr;
	}
}

void UImageCache::AddTexture(const FString& imagePath, UTexture2D* Texture)
{
	TextureCacheMap.Add(imagePath, Texture);
}

