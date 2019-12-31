
#include "Bg2Scene.h"
#include "Bg2Model.h"

#include "CoreMinimal.h"
#include "EngineMinimal.h"

#include "JsonUtilities.h"
#include "ConstructorHelpers.h"
#include "Misc/Paths.h"

#include "Bg2UnrealTools.h"

struct ComponentData {
	FTransform Transform = FTransform::Identity;
	FString DrawablePath;
	// TODO: Other relevant component data

	void ParseComponentData(const TSharedPtr<FJsonObject> & compData, const FString & basePath)
	{
		FString compType = compData->GetStringField("type");
		if (compType == "Transform")
		{
			ParseTransform(compData);
		}
		else if (compType == "Drawable")
		{
			ParseDrawable(compData, basePath);
		}
	}

	void ParseTransform(const TSharedPtr<FJsonObject> & compData)
	{
		const TArray<TSharedPtr<FJsonValue>> * result;
		if (compData->TryGetArrayField("transformMatrix", result) && result->Num()==16)
		{
			float matrix[16];
			for (int32 i = 0; i < result->Num(); ++i)
			{
				matrix[i] = static_cast<float>((*result)[i]->AsNumber());
			}
		}
	}

	void ParseDrawable(const TSharedPtr<FJsonObject> & compData, const FString & basePath)
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
				result.ParseComponentData((*components)[i]->AsObject(), mBasePath);
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
		AActor * nodeActor = mWorld->SpawnActor<AActor>(AActor::StaticClass(), componentData.Transform, params);

		if (componentData.DrawablePath != "")
		{
			// TODO: Load drawable
			UE_LOG(Bg2Tools, Display, TEXT("Load drawable at path: %s"), *componentData.DrawablePath);
			auto mesh = UBg2Model::Load(nodeActor, mBaseMaterial, componentData.DrawablePath, mScale);
			if (mesh)
			{
				mesh->SetupAttachment(nodeActor->GetRootComponent());
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

