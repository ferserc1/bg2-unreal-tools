
#include "Bg2Model.h"

#include "CoreMinimal.h"
#include "ImageLoader.h"
#include "EngineMinimal.h"
#include "JsonUtilities.h"
#include "UObject/ConstructorHelpers.h"
#include "Misc/Paths.h"

#include "Bg2UnrealTools.h"
#include "Bg2Reader.h"
#include "Bg2Material.h"
#include "bg2tools/scene_object.hpp"

#include <map>

UProceduralMeshComponent * UBg2Model::Load(UObject* Outer, UMaterial* BaseMaterial, bg2tools::SceneObject* sceneObject)
{
	UProceduralMeshComponent* result = nullptr;
	std::map<FString, int32> materialIndexes;
	int32 currentMeshIndex = 0;

	if (!sceneObject)
	{
		return nullptr;
	}
	else if (!sceneObject->isValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid scene object found"));
		return nullptr;
	}

	result = NewObject<UProceduralMeshComponent>(Outer, UProceduralMeshComponent::StaticClass(), TEXT("bg2 engine mesh"));
	sceneObject->drawable->applyTransform(sceneObject->worldTransform);

	for (auto pl : sceneObject->drawable->plists)
	{
		TArray<FVector> vertices;
		TArray<int32> Triangles;
		TArray<FVector> normals;
		TArray<FVector2D> UV0;
		TArray<FVector2D> UV1;

		size_t i2 = 0;
		for (size_t i3 = 0; i3 < pl->vertex.size(); i3 += 3, i2 += 2)
		{
			auto x = pl->vertex[i3];
			auto y = pl->vertex[i3 + 1];
			auto z = pl->vertex[i3 + 2];
			auto nx = pl->normal[i3];
			auto ny = pl->normal[i3 + 1];
			auto nz = pl->normal[i3 + 2];

			auto vertex = FVector(x, z, y);
			auto normal = -1.0 * FVector(nx, nz, ny);

			vertices.Add(vertex);
			normals.Add(normal);
			UV0.Add({ pl->uv0[i2], pl->uv0[i2 + 1] });
			UV1.Add({ pl->uv1[i2], pl->uv1[i2 + 1] });
		}

		for (size_t i = 0; i < pl->index.size(); i += 3)
		{
			Triangles.Add(static_cast<int32>(i + 2));
			Triangles.Add(static_cast<int32>(i + 1));
			Triangles.Add(static_cast<int32>(i));
		}

		materialIndexes[pl->matName.c_str()] = currentMeshIndex;
		result->CreateMeshSection_LinearColor(currentMeshIndex, vertices, Triangles, normals, UV0, UV1, UV0, UV0, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
		++currentMeshIndex;
	}

	// Load materials
	FString JsonString = "{\"Materials\":";
	JsonString += FString(sceneObject->drawable->materialData.c_str());
	JsonString += "}";

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	FString basePath;
	FString fileName;
	FString extension;
	FPaths::Split(sceneObject->drawable->modelPath.c_str(), basePath, fileName, extension);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		UE_LOG(Bg2Tools, Display, TEXT("bg2 model: material json parsed OK"));
		TArray<TSharedPtr<FJsonValue>> Materials = JsonObject->GetArrayField("Materials");
		for (int32 matIndex = 0; matIndex < Materials.Num(); ++matIndex)
		{
			const TSharedPtr<FJsonObject> materialItemData = Materials[matIndex]->AsObject();
			FString name = materialItemData->GetStringField("name");
			int32 meshIndex = materialIndexes[name];
			FMaterialProperties materialProperties;
			UMaterialInstanceDynamic* materialInstance = UBg2Material::LoadMaterialWithJsonObject(BaseMaterial, Outer, materialItemData, basePath, materialProperties);
			if (materialInstance)
			{
				result->SetMaterial(meshIndex, materialInstance);
			}
			if (!materialProperties.Visible)
			{
				auto submesh = result->GetProcMeshSection(meshIndex);
				submesh->bSectionVisible = false;
			}
		}
	}

	return result;
}

UProceduralMeshComponent * UBg2Model::Load(UObject * Outer, UMaterial * BaseMaterial, const FString & ModelPath, float Scale)
{
	bg2tools::SceneObject sceneObject;
	sceneObject.drawable = new bg2tools::DrawableData;
	if (sceneObject.drawable->loadDrawable(TCHAR_TO_UTF8(*ModelPath)))
	{
		sceneObject.worldTransform
			.scale({ Scale, Scale, Scale })
			.rotate(bg2tools::radians(90.0f), 1, 0, 0);
		return Load(Outer, BaseMaterial, &sceneObject);
	}
	else {
		// ERROR
		UE_LOG(Bg2Tools, Error, TEXT("Could not load bg2 engine model at path %s"), *ModelPath);
		return nullptr;
	}
}

void UBg2Model::GetExternalResources(const FString & ModelPath, TArray<FString> & Result)
{
	Bg2Reader reader;
	FString MaterialJsonString;
	reader.Materials([&](const std::string & materialData) {
		MaterialJsonString = "{\"Materials\":";
		MaterialJsonString += FString(materialData.c_str());
		MaterialJsonString += "}";
	});

	if (reader.Load(TCHAR_TO_UTF8(*ModelPath)))
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(MaterialJsonString);

		FString basePath;
		FString fileName;
		FString extension;
		FPaths::Split(ModelPath, basePath, fileName, extension);
		TArray<FString> relativePathResources;
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			UE_LOG(Bg2Tools, Display, TEXT("bg2 model: material json parsed OK"));
			TArray<TSharedPtr<FJsonValue>> Materials = JsonObject->GetArrayField("Materials");
			for (int32 matIndex = 0; matIndex < Materials.Num(); ++matIndex)
			{
				const TSharedPtr<FJsonObject> materialItemData = Materials[matIndex]->AsObject();
				TArray<FString> externalResources;
				UBg2Material::GetExternalResources(materialItemData, externalResources);
				for (int i = 0; i < externalResources.Num(); ++i)
				{
					const FString & resource = externalResources[i];
					if (relativePathResources.Find(resource) == INDEX_NONE)
					{
						relativePathResources.Add(resource);
					}
				}
			}
		}

		for (int32 i = 0; i < relativePathResources.Num(); ++i)
		{
			Result.Add(FPaths::Combine(basePath, relativePathResources[i]));
		}
	}
}