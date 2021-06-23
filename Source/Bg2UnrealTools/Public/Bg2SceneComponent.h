#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"


#include "Bg2SceneComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BG2UNREALTOOLS_API UBg2SceneComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBg2SceneComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BG2 Engine")
	float Scale = 100.0f;

	// If not specified, GetOwner() will be used.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BG2 Engine")
	AActor * Owner = nullptr;

	UFUNCTION(BlueprintCallable, Category = "BG2 Engine")
	void LoadScene(FString modelPath);

	UFUNCTION(BlueprintCallable, Category = "BG2 Engine")
	void LoadSceneFromFilesystem();

private:
	FString mScenePath;
	bool mScenePathChanged = false;
	class UMaterial * mBaseMaterial;

	bool DoLoadScene();

};
