#pragma once

#include "GameFramework/Actor.h"

#include "JsonUtilities.h"

#include "Bg2Scene.generated.h"

UCLASS(BlueprintType)
class UBg2Scene : public UObject
{
	GENERATED_BODY()

public:
	static AActor * Load(UObject * Outer, UMaterial * BaseMaterial, const FString & ScenePath, float Scale);
	static AActor * Load(UObject * Outer, UMaterial * BaseMaterial, const TSharedPtr<FJsonObject> & SceneJson, float Scale);
	static void GetExternalResources(const FString & ScenePath, TArray<FString> & Result);
};