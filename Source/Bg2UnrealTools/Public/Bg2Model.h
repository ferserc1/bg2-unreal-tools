#pragma once

#include "ProceduralMeshComponent.h"
#include "bg2tools/scene_object.hpp"
#include "Bg2Model.generated.h"

class UMaterial;

UCLASS(BlueprintType)
class BG2UNREALTOOLS_API UBg2Model : public UObject
{
	GENERATED_BODY()

public:
	static UProceduralMeshComponent* Load(UObject* Outer, UMaterial* BaseMaterial, bg2tools::SceneObject* sceneObject, FVector& origin, FVector& size);
	static UProceduralMeshComponent * Load(UObject * Outer, UMaterial * BaseMaterial, const FString & ModelPath, float Scale, FVector & origin, FVector & size);
	static void GetExternalResources(const FString & ModelPath, TArray<FString> & Result);
};
