
#include "Bg2SceneComponent.h"

#include "Bg2Scene.h"

#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"

#include "EngineMinimal.h"
#include "JsonUtilities.h"
#include "ConstructorHelpers.h"
#include "Misc/Paths.h"

UBg2SceneComponent::UBg2SceneComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

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

void UBg2SceneComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBg2SceneComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (mScenePathChanged)
	{
		if (DoLoadScene())
		{
			// Ok
		}
		else
		{
			// Error
		}

		mScenePathChanged = false;
	}
}

void UBg2SceneComponent::LoadScene(FString modelPath)
{
	mScenePathChanged = modelPath != mScenePath;
	mScenePath = modelPath;
}

void UBg2SceneComponent::LoadSceneFromFilesystem()
{
	if (GEngine)
	{
		if (GEngine->GameViewport)
		{
			FString DialogTitle = "Open Scene";
			FString DefaultPath = FPaths::ProjectContentDir();
			FString FileTypes = "bg2 scene|*.vitscnj";
			TArray<FString> OutFileNames;
			void * ParentWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
			IDesktopPlatform * DesktopPlatform = FDesktopPlatformModule::Get();
			if (DesktopPlatform)
			{
				uint32 SelectionFlag = 0;
				DesktopPlatform->OpenFileDialog(ParentWindowHandle, DialogTitle, DefaultPath, FString(""), FileTypes, SelectionFlag, OutFileNames);
				if (OutFileNames.Num() > 0)
				{
					LoadScene(OutFileNames[0]);
				}
			}
		}
	}
}

bool UBg2SceneComponent::DoLoadScene()
{
	AActor * owner = Owner != nullptr ? Owner : GetOwner();
	bool result = UBg2Scene::Load(owner, mBaseMaterial, mScenePath, Scale);

	TArray<FString> TestResources;
	UBg2Scene::GetExternalResources(mScenePath, TestResources);
	for (int32 i = 0; i < TestResources.Num(); ++i)
	{
		UE_LOG(Bg2Tools, Display, TEXT("Scene external resource: %s"), *TestResources[i]);
	}

	return result;
}