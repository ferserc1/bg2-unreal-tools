// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bg2SceneLoader.generated.h"

UCLASS()
class BG2UNREALTOOLS_API ABg2SceneLoader : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABg2SceneLoader();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void LoadScene(FString Path, float Scale = 100.0f);

	UFUNCTION(BlueprintCallable)
	void LoadSceneFromFilesystem(float Scale = 100.0f);

	UFUNCTION(BlueprintCallable)
	void CloseScene();

	UPROPERTY()
	AActor* SceneRoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMaterial * BaseMaterial;

};
