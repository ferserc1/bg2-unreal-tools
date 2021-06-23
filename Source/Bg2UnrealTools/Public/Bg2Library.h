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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bg2 Library")
	FString Version;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bg2 Library")
	UBg2LibraryNode * Root;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bg2 Library")
	FString BasePath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bg2 Library")
	FString Name;

	static UBg2Library* Load(UObject* Owner, const FString& LibraryPath);
	static UBg2Library* Load(UObject* Owner, const TSharedPtr<FJsonObject>& LibraryJson, const FString& BasePath, const FString& LibraryName);
	static bool GetExternalResources(const FString& LibraryPath, TArray<FString>& Result);
};

