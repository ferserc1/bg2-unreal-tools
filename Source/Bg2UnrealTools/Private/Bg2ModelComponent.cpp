// Fill out your copyright notice in the Description page of Project Settings.


#include "Bg2ModelComponent.h"

#include "Bg2UnrealTools.h"
#include "Bg2Reader.h"
#include "Bg2Material.h"
#include "Bg2Model.h"
#include "ImageLoader.h"


#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"

#include "EngineMinimal.h"
#include "JsonUtilities.h"
#include "ConstructorHelpers.h"
#include "Misc/Paths.h"

// Sets default values for this component's properties
UBg2ModelComponent::UBg2ModelComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ModelMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	ModelMesh->bUseAsyncCooking = true; // multi-threaded PhysX cooking

	ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("/Bg2UnrealTools/Materials/TestMaterial"));
	if (MaterialFinder.Succeeded())
	{
		mBaseMaterial = MaterialFinder.Object;
	}
	else
	{
		mBaseMaterial = CreateDefaultSubobject<UMaterial>(TEXT("InvalidBaseMaterial"));
	}
}


// Called when the game starts
void UBg2ModelComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBg2ModelComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (mModelPathChanged)
	{
		if (LoadModelMesh())
		{
			// Ok
		}
		else
		{
			// Error
		}

		mModelPathChanged = false;
	}
}

void UBg2ModelComponent::LoadModel(FString modelPath)
{
	mModelPathChanged = modelPath != mModelPath;
	mModelPath = modelPath;
}

void UBg2ModelComponent::LoadModelFromFilesystem()
{
	if (GEngine)
	{
		if (GEngine->GameViewport)
		{
			FString DialogTitle = "Open file";
			FString DefaultPath = FPaths::ProjectContentDir();
			FString FileTypes = "bg2 files|*.bg2";
			TArray<FString> OutFileNames;
			void * ParentWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
			IDesktopPlatform * DesktopPlatform = FDesktopPlatformModule::Get();
			if (DesktopPlatform)
			{
				uint32 SelectionFlag = 0;
				DesktopPlatform->OpenFileDialog(ParentWindowHandle, DialogTitle, DefaultPath, FString(""), FileTypes, SelectionFlag, OutFileNames);
				if (OutFileNames.Num() > 0)
				{
					LoadModel(OutFileNames[0]);
				}
			}
		}
	}
}

bool UBg2ModelComponent::LoadModelMesh()
{
	auto bg2Mesh = UBg2Model::Load(this, mBaseMaterial, mModelPath, Scale);
	if (bg2Mesh)
	{
		ModelMesh->DestroyComponent();

		ModelMesh = bg2Mesh;
		ModelMesh->SetupAttachment(GetOwner()->GetRootComponent());
		ModelMesh->RegisterComponent();
		return true;
	}
	else
	{
		return false;
	}
	/*
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
		for (size_t i = 0; i < index.size(); i+=3)
		{
			Triangles.Add(static_cast<int32>(i + 2));
			Triangles.Add(static_cast<int32>(i + 1));
			Triangles.Add(static_cast<int32>(i));
		}
		mMaterialIndexes[currentMatName] = currentMeshIndex;
		ModelMesh->CreateMeshSection_LinearColor(currentMeshIndex, vertices, Triangles, normals, UV0, UV1, UV0, UV0, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
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

	ModelMesh->DestroyComponent(true);

	ModelMesh = NewObject<UProceduralMeshComponent>(this, UProceduralMeshComponent::StaticClass(), TEXT("BG2E mesh"));
	if (reader.Load(TCHAR_TO_UTF8(*mModelPath)))
	{
		LoadMaterials(materialData);
		ModelMesh->SetupAttachment(GetOwner()->GetRootComponent());
		ModelMesh->RegisterComponent();
		return true;
	}
	else
	{
		UE_LOG(Bg2Tools, Fatal, TEXT("Could not load bg2 model."));
		return false;
	}
	*/
}

void UBg2ModelComponent::LoadMaterials(const std::string & materialData)
{
	FString JsonString = "{\"Materials\":";
	JsonString += FString(materialData.c_str());
	JsonString += "}";
	
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	FString basePath;
	FString fileName;
	FString extension;
	FPaths::Split(mModelPath, basePath, fileName, extension);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		UE_LOG(Bg2Tools, Display, TEXT("bg2 model: material json parsed OK"));
		TArray<TSharedPtr<FJsonValue>> Materials = JsonObject->GetArrayField("Materials");
		for (int32 matIndex = 0; matIndex < Materials.Num(); ++matIndex)
		{
			const TSharedPtr<FJsonObject> materialItemData = Materials[matIndex]->AsObject();
			FString name = materialItemData->GetStringField("name");
			int32 meshIndex = mMaterialIndexes[name];
			UMaterialInstanceDynamic * materialInstance = UBg2Material::LoadMaterialWithJsonObject(mBaseMaterial, this, materialItemData, basePath);
			if (materialInstance)
			{
				ModelMesh->SetMaterial(meshIndex, materialInstance);
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

