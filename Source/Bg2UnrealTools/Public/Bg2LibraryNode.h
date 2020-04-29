// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Bg2LibraryNode.generated.h"


UENUM()
enum Bg2NodeType {
	Group UMETA(DisplayName = "Group"),
	Model UMETA(DisplayName = "Model"),
	Material UMETA(DisplayName = "Material")
};

UCLASS()
class BG2UNREALTOOLS_API UBg2LibraryNode : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bg2 Library")
	TEnumAsByte<Bg2NodeType> Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bg2 Library")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bg2 Library")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bg2 Library")
	FString File;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bg2 Library")
	FString Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bg2 Library")
	TMap<FString,FString> Metadata;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bg2 Library")
	TArray<UBg2LibraryNode*> Children;
};
