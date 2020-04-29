// Fill out your copyright notice in the Description page of Project Settings.


#include "Bg2Library.h"

#include "Bg2UnrealTools.h"

class LibraryParser {
public:
	LibraryParser(UObject * Own, const TSharedPtr<FJsonObject>& Obj)
		:mOwner(Own)
		,mJsonObject(Obj)
	{

	}

	UBg2Library* Load()
	{
		UBg2Library* library = NewObject<UBg2Library>(mOwner, UBg2Library::StaticClass());
		library->Root = NewObject<UBg2LibraryNode>(library, UBg2LibraryNode::StaticClass());

		ParseNodeArray("root", mJsonObject, library->Root);

		return library;
	}

private:
	UObject* mOwner;
	const TSharedPtr<FJsonObject> mJsonObject;

	bool ParseNodeArray(const FString& attrName, const TSharedPtr<FJsonObject>& nodeData, UBg2LibraryNode* parentNode)
	{
		const TArray<TSharedPtr<FJsonValue>>* nodes;
		if (nodeData->TryGetArrayField(attrName, nodes))
		{
			for (int32 i = 0; i < nodes->Num(); ++i)
			{
				auto child = (*nodes)[i]->AsObject();
				auto childNode = ParseNode(child);
				if (childNode)
				{
					parentNode->Children.Add(childNode);
				}
				else {
					return false;
				}
			}
		}

		return true;
	}

	UBg2LibraryNode * ParseNode(const TSharedPtr<FJsonObject>& nodeData)
	{
		UBg2LibraryNode* result = NewObject<UBg2LibraryNode>(mOwner, UBg2LibraryNode::StaticClass());

		// Type
		FString stringValue;
		if (nodeData->TryGetStringField("type", stringValue))
		{
			if (stringValue == "group")
			{
				result->Type = Bg2NodeType::Group;
			}
			else if (stringValue == "model")
			{
				result->Type = Bg2NodeType::Model;
			}
			else if (stringValue == "material")
			{
				result->Type = Bg2NodeType::Material;
			}
		}
		else
		{
			result->Type = Bg2NodeType::Group;
		}

		nodeData->TryGetStringField("id", result->Id);
		nodeData->TryGetStringField("name", result->Name);
		nodeData->TryGetStringField("icon", result->Icon);

		if (result->Type == Bg2NodeType::Model)
		{
			nodeData->TryGetStringField("file", result->File);
		}

		// TODO: Materials

		// TODO: Metadata

		// Children
		if (result->Type == Bg2NodeType::Group && !ParseNodeArray("children", nodeData, result))
		{
			return nullptr;
		}

		return result;
	}
};

UBg2Library* UBg2Library::Load(UObject* Owner, const FString& LibraryPath)
{
	FString JsonString;
	
	if (!FFileHelper::LoadFileToString(JsonString, *LibraryPath))
	{
		UE_LOG(Bg2Tools, Error, TEXT("Failed to load bg2 library. No such file at path %s"), *LibraryPath);
		return nullptr;
	}

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		FString basePath;
		FString fileName;
		FString extension;
		FPaths::Split(LibraryPath, basePath, fileName, extension);
		return Load(Owner, JsonObject, basePath, fileName);
	}
	else
	{
		return nullptr;
	}
}

UBg2Library* UBg2Library::Load(UObject* Owner, const TSharedPtr<FJsonObject>& LibraryJson, const FString& BasePath, const FString& LibraryName)
{
	LibraryParser parser(Owner, LibraryJson);
	auto library = parser.Load();
	library->BasePath = BasePath;
	library->Name = LibraryName;
	return library;
}

bool UBg2Library::GetExternalResources(const FString& LibraryPath, TArray<FString>& Result)
{
	return false;
}

