// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Bg2LibraryNode.h"
#include "JsonUtilities.h"
#include "Bg2Library.generated.h"

/**
 * 
 */
UCLASS()
class BG2UNREALTOOLS_API UBg2Library : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bg2 Library")
	FString Version;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bg2 Library")
	TArray<UBg2LibraryNode*> Root;

	static UBg2Library* Load(const FString& LibraryPath);
	static UBg2Library* Load(const TSharedPtr<FJsonObject>& SceneJson);
	static bool GetExternalResources(const FString& LibraryPath, TArray<FString>& Result);
};

