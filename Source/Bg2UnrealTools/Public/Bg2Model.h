#pragma once

#include "ProceduralMeshComponent.h"

#include "Bg2Model.generated.h"

class UMaterial;

UCLASS(BlueprintType)
class UBg2Model : public UObject
{
	GENERATED_BODY()

public:
	static UProceduralMeshComponent * Load(UObject * Outer, UMaterial * BaseMaterial, const FString & ModelPath, float Scale);

};
