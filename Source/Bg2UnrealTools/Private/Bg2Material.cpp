
#include "Bg2Material.h"

#include "CoreMinimal.h"
#include "ImageLoader.h"
#include "EngineMinimal.h"
#include "JsonUtilities.h"
#include "ConstructorHelpers.h"
#include "Misc/Paths.h"

UMaterialInstanceDynamic * UBg2Material::LoadMaterialWithJsonString(UMaterial * BaseMaterial, UObject * Outer, const FString & JsonString, const FString & BasePath)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		return LoadMaterialWithJsonObject(BaseMaterial, Outer, JsonObject, BasePath);
	}
	else
	{
		return nullptr;
	}
}

UMaterialInstanceDynamic * UBg2Material::LoadMaterialWithJsonObject(UMaterial * BaseMaterial, UObject * Outer, const TSharedPtr<FJsonObject> & JsonObject, const FString & BasePath)
{
	UMaterialInstanceDynamic * result = nullptr;

	// TODO: Check that the material type is PBR
	// FString type = JsonObject->GetStringField("class");
	
	result = UMaterialInstanceDynamic::Create(BaseMaterial, Outer);

	// TODO: Refactor read functions
	FString diffuseTexture;
	const TArray<TSharedPtr<FJsonValue>> * diffuseColor;
	if (JsonObject->TryGetStringField("diffuse", diffuseTexture))
	{
		FString fullPath = FPaths::Combine(BasePath, diffuseTexture);
		UTexture2D * texture = UImageLoader::LoadImageFromDisk(Outer, fullPath);
		result->SetTextureParameterValue(TEXT("DiffuseTexture"), texture);
	}
	else if (JsonObject->TryGetArrayField("diffuse", diffuseColor))
	{
		float components[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		for (int32 componentIndex = 0; componentIndex < diffuseColor->Num() && componentIndex < 4; ++componentIndex)
		{
			components[componentIndex] = static_cast<float>((*diffuseColor)[componentIndex]->AsNumber());
		}
		result->SetVectorParameterValue(TEXT("DiffuseColor"), FLinearColor(components[0], components[1], components[2], components[3]));
	}
	

	return result;
}
