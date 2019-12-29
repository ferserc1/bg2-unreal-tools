
#include "Bg2Scene.h"

#include "CoreMinimal.h"
#include "EngineMinimal.h"

#include "JsonUtilities.h"
#include "ConstructorHelpers.h"
#include "Misc/Paths.h"

#include "Bg2UnrealTools.h"

class SceneParser {
public:
	SceneParser(UObject * Outer, const TSharedPtr<FJsonObject> & Obj, const FString & BasePath) :mOuter(Outer), mJsonObject(Obj), mBasePath(BasePath) {}

protected:
	UObject * mOuter;
	const TSharedPtr<FJsonObject> mJsonObject;
	FString mBasePath;

};

AActor * UBg2Scene::Load(UObject * Outer, UMaterial * BaseMaterial, const FString & ScenePath, float Scale)
{
	FString JsonString;

	if (!FFileHelper::LoadFileToString(JsonString, *ScenePath))
	{
		UE_LOG(Bg2Tools, Error, TEXT("Failed to load bg2 engine scene. No such file at path : %s"), *ScenePath);
		return nullptr;
	}

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
	
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		return Load(Outer, BaseMaterial, JsonObject, Scale);
	}
	else
	{
		UE_LOG(Bg2Tools, Error, TEXT("Failed to load bg2 engine scene. Scene parse error in file %s"), *ScenePath);
		return nullptr;
	}

}

AActor * UBg2Scene::Load(UObject * Outer, UMaterial * BaseMaterial, const TSharedPtr<FJsonObject> & SceneJson, float Scale)
{
	UE_LOG(Bg2Tools, Error, TEXT("Bg2 engine scene load not implemented"));
	return nullptr;
}

void UBg2Scene::GetExternalResources(const FString & ScenePath, TArray<FString> & Result)
{

}

