// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralMeshComponent.h"

#include <string>
#include "JsonUtilities.h"

#include "Bg2ModelComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BG2UNREALTOOLS_API UBg2ModelComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBg2ModelComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent * ModelMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BG2 Engine")
	float Scale = 100.0f;

	UFUNCTION(BlueprintCallable, Category = "BG2 Engine")
	void LoadModel(FString modelPath);

	UFUNCTION(BlueprintCallable, Category = "BG2 Engine")
	void LoadModelFromFilesystem();

private:
	FString mModelPath;
	bool mModelPathChanged = false;

	bool LoadModelMesh();
	void LoadMaterials(const std::string & materialData);
	void LoadMaterial(const TSharedPtr<FJsonObject> & materialData);
};
