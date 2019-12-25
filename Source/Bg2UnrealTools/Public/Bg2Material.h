#pragma once

#include "JsonUtilities.h"
#include "Templates/SharedPointer.h"
#include "Bg2Material.generated.h"

class UMaterialInstanceDynamic;
class UMaterial;

UCLASS(BlueprintType)
class UBg2Material : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Bg2 Engine", meta = (HidePin = "Outer", DefaultToSelf = "Outer"))
	static UMaterialInstanceDynamic * LoadMaterialWithJsonString(UMaterial * BaseMaterial, UObject * Outer, const FString & JsonString, const FString & BasePath);

	static UMaterialInstanceDynamic * LoadMaterialWithJsonObject(UMaterial * BaseMaterial, UObject * Outer, const TSharedPtr<FJsonObject> & JsonObject, const FString & BasePath);

	static void GetExternalResources(const TSharedPtr<FJsonObject> & JsonObject, TArray<FString> & Result);

};
