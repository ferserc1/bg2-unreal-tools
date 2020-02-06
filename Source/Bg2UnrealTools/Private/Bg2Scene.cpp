
#include "Bg2Scene.h"
#include "Bg2Model.h"

#include "CoreMinimal.h"
#include "EngineMinimal.h"

#include "JsonUtilities.h"
#include "ConstructorHelpers.h"
#include "Misc/Paths.h"

#include "Bg2UnrealTools.h"

#include "bg2tools/matrix.hpp"

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
	SceneParser(UWorld * World, AActor * RootActor, UMaterial * BaseMaterial, const TSharedPtr<FJsonObject> & Obj, const FString & BasePath, float Scale)
		:mWorld(World), mRootActor(RootActor), mBaseMaterial(BaseMaterial), mJsonObject(Obj), mBasePath(BasePath), mScale(Scale) {}


	bool LoadScene()
	{
		ParseNodeArray("scene", mJsonObject, mRootActor);
		// TODO: Error handling
		return true;
	}

	void ParseNodeArray(const FString & attrName, const TSharedPtr<FJsonObject> & nodeData, AActor * parentActor)
	{
		const TArray<TSharedPtr<FJsonValue>> * nodes;

		if (nodeData->TryGetArrayField(attrName, nodes))
		{
			for (int32 i = 0; i < nodes->Num(); ++i)
			{
				auto child = (*nodes)[i]->AsObject();
				ParseNode(child, parentActor);
			}
		}
	}

	void ParseComponents(const TSharedPtr<FJsonObject> & nodeData, ComponentData & result)
	{
		const TArray<TSharedPtr<FJsonValue>> * components;
		if (nodeData->TryGetArrayField("components", components))
		{
			for (int32 i = 0; i < components->Num(); ++i)
			{
				result.ParseComponentData((*components)[i]->AsObject(), mBasePath, mScale);
			}
		}
	}

	void ParseNode(const TSharedPtr<FJsonObject> & node, AActor * ownerNode)
	{
		ComponentData componentData;
		FName actorName;
		FString nameString;
		if (!node->TryGetStringField("name", nameString))
		{
			actorName = "bg2 node actor";
		}
		else
		{
			actorName = FName(*nameString);
		}

		ParseComponents(node, componentData);

		FActorSpawnParameters params;
		params.Name = actorName;
		params.Owner = ownerNode;
		componentData.Transform.MultiplyScale3D({ mScale, mScale, mScale });
		AActor * nodeActor = mWorld->SpawnActor<AActor>(AActor::StaticClass(), componentData.Transform, params);

		if (componentData.DrawablePath != "")
		{
			// TODO: Load drawable
			UE_LOG(Bg2Tools, Display, TEXT("Load drawable at path: %s"), *componentData.DrawablePath);
			auto mesh = UBg2Model::Load(nodeActor, mBaseMaterial, componentData.DrawablePath, 1.0f);
			if (mesh)
			{
				mesh->SetupAttachment(nodeActor->GetRootComponent());
				mesh->SetAbsolute(true, true, true);
				mesh->SetWorldTransform(componentData.Transform);
				mesh->RegisterComponent();
			}
		}

		ParseNodeArray("children", node, nodeActor);
	}

protected:
	UWorld * mWorld;
	AActor * mRootActor;
	UMaterial * mBaseMaterial;
	const TSharedPtr<FJsonObject> mJsonObject;
	FString mBasePath;
	float mScale;
};

bool UBg2Scene::Load(AActor * RootActor, UMaterial * BaseMaterial, const FString & ScenePath, float Scale)
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
		return Load(RootActor, BaseMaterial, JsonObject, basePath, Scale);
	}
	else
	{
		UE_LOG(Bg2Tools, Error, TEXT("Failed to load bg2 engine scene. Scene parse error in file %s"), *ScenePath);
		return false;
	}
}

bool UBg2Scene::Load(AActor * RootActor, UMaterial * BaseMaterial, const TSharedPtr<FJsonObject> & SceneJson, const FString & BasePath, float Scale)
{
	SceneParser parser(RootActor->GetWorld(), RootActor, BaseMaterial, SceneJson, BasePath, Scale);
	return parser.LoadScene();
}

void UBg2Scene::GetExternalResources(const FString & ScenePath, TArray<FString> & Result)
{

}

