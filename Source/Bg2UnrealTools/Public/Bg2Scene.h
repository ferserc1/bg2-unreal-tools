#pragma once

#include "GameFramework/Actor.h"

#include "JsonUtilities.h"
#include "materials/material.h"

#include "Bg2Scene.generated.h"

UCLASS(BlueprintType)
class UBg2Scene : public UObject
{
	GENERATED_BODY()

public:
	static bool Load(AActor * RootActor, UMaterial * BaseMaterial, const FString & ScenePath, float Scale);
	static bool Load(AActor * RootActor, UMaterial * BaseMaterial, const TSharedPtr<FJsonObject> & SceneJson, const FString & BasePath, float Scale);
	static bool GetExternalResources(const FString & ScenePath, TArray<FString> & Result);
};