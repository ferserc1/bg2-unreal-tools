
#include "Bg2Scene.h"
#include "Bg2Model.h"

#include "CoreMinimal.h"
#include "EngineMinimal.h"

#include "JsonUtilities.h"
#include "UObject/ConstructorHelpers.h"
#include "Misc/Paths.h"

#include "Bg2UnrealTools.h"
#include "Bg2Model.h"

#include "bg2tools/matrix.hpp"
#include "bg2tools/bg2_scene.hpp"

struct ComponentData {
	FTransform Transform = FTransform::Identity;
	FString DrawablePath;
	// TODO: Other relevant component data

	void ParseComponentData(const TSharedPtr<FJsonObject> & compData, const FString & basePath, float scale)
	{
		FString compType = compData->GetStringField("type");
		if (compType == "Transform")
		{
			ParseTransform(compData, scale);
		}
		else if (compType == "Drawable")
		{
			ParseDrawable(compData, basePath, scale);
		}
	}

	void ParseTransform(const TSharedPtr<FJsonObject> & compData, float scale)
	{
		const TArray<TSharedPtr<FJsonValue>> * result;
		if (compData->TryGetArrayField("transformMatrix", result) && result->Num()==16)
		{
			float m[16];
			for (int32 i = 0; i < result->Num(); ++i)
			{
				m[i] = static_cast<float>((*result)[i]->AsNumber());
			}

			
			FMatrix mat;
			mat.SetIdentity();
			UE_LOG(LogTemp, Warning, TEXT("Scale: %f"), scale);
			mat.M[0][0] = m[ 0]; mat.M[0][1] = m[ 2]; mat.M[0][2] = m[ 1]; mat.M[0][3] = m[ 3];
			mat.M[1][0] = m[ 4]; mat.M[1][1] = m[ 6]; mat.M[1][2] = m[ 5]; mat.M[1][3] = m[ 7];
			mat.M[2][0] = m[ 8]; mat.M[2][1] = m[10]; mat.M[2][2] = m[ 9]; mat.M[2][3] = m[11];
			mat.M[3][0] = m[12]; mat.M[3][1] = m[14]; mat.M[3][2] = m[13]; mat.M[3][3] = m[15];
			mat.ApplyScale(scale);

			Transform.SetFromMatrix(mat);
		}
		else
		{
			FMatrix mat;
			mat.SetIdentity();
			mat.ApplyScale(scale);
			Transform.SetFromMatrix(mat);
		}
	}

	void ParseDrawable(const TSharedPtr<FJsonObject> & compData, const FString & basePath, float scale)
	{
		FString drawableName;
		if (compData->TryGetStringField("name", drawableName))
		{
			DrawablePath = FPaths::Combine(basePath, drawableName);
			if (FPaths::FileExists(DrawablePath + ".vwglb"))
			{
				DrawablePath.Append(".vwglb");
			}
			else if (FPaths::FileExists(DrawablePath + ".bg2"))
			{
				DrawablePath.Append(".bg2");
			}
		}

	}
};

class SceneParser {
public:
	SceneParser(UWorld* World, AActor* RootActor, UMaterial* BaseMaterial, const TSharedPtr<FJsonObject>& Obj, const FString& BasePath, float Scale, FVector Offset = FVector{ 0.f, 0.f, 0.f })
		:mWorld(World), mRootActor(RootActor), mBaseMaterial(BaseMaterial), mJsonObject(Obj), mBasePath(BasePath), mScale(Scale), mOffset(Offset) {}

	// Use this constructor only to create a parser to read scene resources
	SceneParser(const TSharedPtr<FJsonObject>& Obj, const FString& BasePath)
		:mWorld(nullptr), mRootActor(nullptr), mBaseMaterial(nullptr), mJsonObject(Obj), mBasePath(BasePath), mScale(100.0f), mOffset(FVector{0.f, 0.f, 0.f}) {}

	bool GetExternalResources(TArray<FString> & Result)
	{
		ParseNodeArray("scene", mJsonObject);
		for (auto node : mScene.sceneObjects())
		{
			if (node->drawable)
			{
				Result.Add(node->drawable->modelPath.c_str());
			}
		}
		return true;
	}

	bool LoadScene()
	{
		ParseNodeArray("scene", mJsonObject);
		for (auto node : mScene.sceneObjects())
		{
			FActorSpawnParameters params;
			params.Name = FName(node->name.c_str());
			params.Owner = mRootActor;
			AActor* nodeActor = mWorld->SpawnActor<AActor>(AActor::StaticClass(), params);

			node->worldTransform = 
				//bg2tools::float4x4::Rotation(bg2tools::radians(90.0f), 1, 0, 0) *
				bg2tools::float4x4::Translation({ static_cast<float>(mOffset.X), static_cast<float>(mOffset.Z), static_cast<float>(mOffset.Y) }) *
				bg2tools::float4x4::Scale({ mScale, mScale, mScale }) *
				node->worldTransform;

			auto mesh = UBg2Model::Load(nodeActor, mBaseMaterial, node);
			if (mesh)
			{
				nodeActor->SetActorEnableCollision(false);
				mesh->SetupAttachment(nodeActor->GetRootComponent());
				mesh->RegisterComponent();
			}
		}
		return false;
	}

	void ParseNodeArray(const FString& attrName, const TSharedPtr<FJsonObject>& nodeData)
	{
		const TArray<TSharedPtr<FJsonValue>>* nodes;
		if (nodeData->TryGetArrayField(attrName, nodes))
		{
			for (int32 i = 0; i < nodes->Num(); ++i)
			{
				auto child = (*nodes)[i]->AsObject();
				ParseNode(child);
			}
		}
	}

	void ParseNode(const TSharedPtr<FJsonObject>& nodeData)
	{
		mScene.pushMatrix();
		FString nameString;
		if (nodeData->TryGetStringField("name", nameString))
		{
			mScene.setCurrentName(TCHAR_TO_UTF8(*nameString));
		}
		else
		{
			mScene.setCurrentName("untitled node");
		}

		ParseComponents(nodeData);

		ParseNodeArray("children", nodeData);
		mScene.tryAddNode();
		mScene.popMatrix();
	}

	void ParseComponents(const TSharedPtr<FJsonObject>& nodeData)
	{
		const TArray<TSharedPtr<FJsonValue>>* components;
		if (nodeData->TryGetArrayField("components", components))
		{
			for (int32 i = 0; i < components->Num(); ++i)
			{
				ParseComponentData((*components)[i]->AsObject());
			}
		}
	}

	void ParseComponentData(const TSharedPtr<FJsonObject>& componentData)
	{
		FString compType = componentData->GetStringField("type");
		if (compType == "Transform")
		{
			ParseTransform(componentData);
		}
		else if (compType == "Drawable")
		{
			ParseDrawable(componentData);
		}
	}

	void ParseTransform(const TSharedPtr<FJsonObject>& transformData)
	{
		const TArray<TSharedPtr<FJsonValue>>* result;
		if (transformData->TryGetArrayField("transformMatrix", result) && result->Num() == 16)
		{
			float m[16];
			for (int32 i = 0; i < result->Num(); ++i)
			{
				m[i] = static_cast<float>((*result)[i]->AsNumber());
			}

			bg2tools::float4x4 matrix(
				m[ 0], m[ 1], m[ 2], m[ 3],
				m[ 4], m[ 5], m[ 6], m[ 7],
				m[ 8], m[ 9], m[10], m[11],
				m[12], m[13], m[14], m[15]
			);

			mScene.multMatrix(matrix);
		}
	}

	void ParseDrawable(const TSharedPtr<FJsonObject>& drawableData)
	{
		FString drawableName;
		if (drawableData->TryGetStringField("name", drawableName))
		{
			FString drawablePath = FPaths::Combine(mBasePath, drawableName);
			if (FPaths::FileExists(drawablePath + ".vwglb"))
			{
				drawablePath.Append(".vwglb");
			}
			else if (FPaths::FileExists(drawablePath + ".bg2"))
			{
				drawablePath.Append(".bg2");
			}
			else {
				return;
			}

			auto drw = new bg2tools::DrawableData();
			if (drw->loadDrawable(TCHAR_TO_UTF8(*drawablePath)))
			{
				mScene.setCurrentDrawable(drw);
			}
		}
	}

protected:
	UWorld* mWorld;
	AActor* mRootActor;
	UMaterial* mBaseMaterial;
	const TSharedPtr<FJsonObject> mJsonObject;
	FString mBasePath;
	float mScale;
	FVector mOffset;

	bg2tools::Bg2Scene mScene;
};


bool UBg2Scene::Load(AActor * RootActor, UMaterial * BaseMaterial, const FString & ScenePath, float Scale, FVector Offset)
{
	FString JsonString;

	if (!FFileHelper::LoadFileToString(JsonString, *ScenePath))
	{
		UE_LOG(Bg2Tools, Error, TEXT("Failed to load bg2 engine scene. No such file at path : %s"), *ScenePath);
		return false;
	}

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
	
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		FString basePath;
		FString fileName;
		FString extension;
		FPaths::Split(ScenePath, basePath, fileName, extension);
		return Load(RootActor, BaseMaterial, JsonObject, basePath, Scale, Offset);
	}
	else
	{
		UE_LOG(Bg2Tools, Error, TEXT("Failed to load bg2 engine scene. Scene parse error in file %s"), *ScenePath);
		return false;
	}
}

bool UBg2Scene::Load(AActor * RootActor, UMaterial * BaseMaterial, const TSharedPtr<FJsonObject> & SceneJson, const FString & BasePath, float Scale, FVector Offset)
{
	SceneParser parser(RootActor->GetWorld(), RootActor, BaseMaterial, SceneJson, BasePath, Scale, Offset);
	return parser.LoadScene();
}

bool UBg2Scene::GetExternalResources(const FString & ScenePath, TArray<FString> & Result)
{
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *ScenePath))
	{
		UE_LOG(Bg2Tools, Error, TEXT("Failed to load bg2 engine scene. Nu such file at path %s"), *ScenePath);
		return false;
	}

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		FString basePath;
		FString fileName;
		FString extension;
		FPaths::Split(ScenePath, basePath, fileName, extension);

		// TODO: Implement this
		SceneParser parser(JsonObject, basePath);
		parser.GetExternalResources(Result);
		return true;
	}
	else
	{
		UE_LOG(Bg2Tools, Error, TEXT("Failed to load bg2 engine scene. Scene parse error in file %s"), *ScenePath);
		return false;
	}
}

