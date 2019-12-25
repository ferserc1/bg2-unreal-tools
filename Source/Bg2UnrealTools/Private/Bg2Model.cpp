
#include "Bg2Model.h"

#include "CoreMinimal.h"
#include "ImageLoader.h"
#include "EngineMinimal.h"
#include "JsonUtilities.h"
#include "ConstructorHelpers.h"
#include "Misc/Paths.h"

#include "Bg2UnrealTools.h"
#include "Bg2UnrealTools.h"
#include "Bg2Reader.h"
#include "Bg2Material.h"

#include <map>

UProceduralMeshComponent * UBg2Model::Load(UObject * Outer, UMaterial * BaseMaterial, const FString & ModelPath, float Scale)
{
	UProceduralMeshComponent * result = nullptr;
	std::map<FString, int32> materialIndexes;

	Bg2Reader reader;

	TArray<FVector> vertices;
	TArray<int32> Triangles;
	TArray<FVector> normals;
	TArray<FVector2D> UV0;
	TArray<FVector2D> UV1;

	FString currentPlistName;
	FString currentMatName;
	int32 currentMeshIndex = 0;
	std::string materialData;

	reader.Version([&](uint8_t, uint8_t, uint8_t) {

	})
	.Materials([&](const std::string & matData) {
		materialData = matData;
	})
	.PlistName([&](const std::string & plistName) {
		currentPlistName = plistName.c_str();
	})
	.MaterialName([&](const std::string & matName) {
		currentMatName = matName.c_str();
	})
	.Vertex([&](const std::vector<float> & v) {
		for (size_t i = 0; i < v.size(); i += 3)
		{
			FVector vector(v[i] * Scale, v[i + 1] * Scale, v[i + 2] * Scale);
			vertices.Add(vector.RotateAngleAxis(90.0f, { 1.0f, 0.0f, 0.0f }));
		}
	})
	.Normal([&](const std::vector<float> & n) {
		for (size_t i = 0; i < n.size(); i += 3)
		{
			normals.Add(FVector(n[i], n[i + 1], n[i + 2]).RotateAngleAxis(90.0f, { 1.0f, 0.0f, 0.0f }));
		}
	})
	.Uv0([&](const std::vector<float> & t) {
		for (size_t i = 0; i < t.size(); i += 2)
		{
			UV0.Add(FVector2D(t[i], t[i + 1]));
		}
	})
	.Uv1([&](const std::vector<float> & t) {
		for (size_t i = 0; i < t.size(); i += 2)
		{
			UV1.Add(FVector2D(t[i], t[i + 1]));
		}
	})
	.Index([&](const std::vector<unsigned int> & index) {
		// Create mesh
		for (size_t i = 0; i < index.size(); i += 3)
		{
			Triangles.Add(static_cast<int32>(i + 2));
			Triangles.Add(static_cast<int32>(i + 1));
			Triangles.Add(static_cast<int32>(i));
		}
		materialIndexes[currentMatName] = currentMeshIndex;
		result->CreateMeshSection_LinearColor(currentMeshIndex, vertices, Triangles, normals, UV0, UV1, UV0, UV0, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
		++currentMeshIndex;
		vertices.Empty();
		normals.Empty();
		UV0.Empty();
		UV1.Empty();
		Triangles.Empty();
	})
	.Error([&](const std::exception & e) {
		UE_LOG(Bg2Tools, Error, TEXT("Error loading bg2 model: %s"), e.what());
	});

	result = NewObject<UProceduralMeshComponent>(Outer, UProceduralMeshComponent::StaticClass(), TEXT("Bg2 Engine Mesh"));
	if (reader.Load(TCHAR_TO_UTF8(*ModelPath)))
	{
		// Load materials
		FString JsonString = "{\"Materials\":";
		JsonString += FString(materialData.c_str());
		JsonString += "}";

		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

		FString basePath;
		FString fileName;
		FString extension;
		FPaths::Split(ModelPath, basePath, fileName, extension);

		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			UE_LOG(Bg2Tools, Display, TEXT("bg2 model: material json parsed OK"));
			TArray<TSharedPtr<FJsonValue>> Materials = JsonObject->GetArrayField("Materials");
			for (int32 matIndex = 0; matIndex < Materials.Num(); ++matIndex)
			{
				const TSharedPtr<FJsonObject> materialItemData = Materials[matIndex]->AsObject();
				FString name = materialItemData->GetStringField("name");
				int32 meshIndex = materialIndexes[name];
				UMaterialInstanceDynamic * materialInstance = UBg2Material::LoadMaterialWithJsonObject(BaseMaterial, Outer, materialItemData, basePath);
				if (materialInstance)
				{
					result->SetMaterial(meshIndex, materialInstance);
				}

				TArray<FString> externalResources;
				UBg2Material::GetExternalResources(materialItemData, externalResources);
				for (int i = 0; i < externalResources.Num(); ++i)
				{
					UE_LOG(Bg2Tools, Display, TEXT("Resource: %s"), *externalResources[i]);
				}
			}
		}
	}
	else
	{
		UE_LOG(Bg2Tools, Fatal, TEXT("Could not load bg2 model"));
		result->DestroyComponent();
		result = nullptr;
	}
	return result;
}