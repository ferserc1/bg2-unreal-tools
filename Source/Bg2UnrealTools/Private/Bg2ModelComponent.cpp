// Fill out your copyright notice in the Description page of Project Settings.


#include "Bg2ModelComponent.h"

#include "Bg2UnrealTools.h"
#include "Bg2Reader.h"
#include "Bg2Material.h"
#include "Bg2Model.h"
#include "ImageLoader.h"
#include "Engine.h"

#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"

#include "EngineMinimal.h"
#include "JsonUtilities.h"
#include "UObject/ConstructorHelpers.h"
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
			FString FileTypes = "bg2 files|*.bg2;vwglb files|*.vwglb";
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

	TArray<FString> TestResources;
	UBg2Model::GetExternalResources(mModelPath, TestResources);
	for (int32 i = 0; i < TestResources.Num(); ++i)
	{
		UE_LOG(Bg2Tools, Display, TEXT("External resource: %s"), *TestResources[i]);
	}

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
}
