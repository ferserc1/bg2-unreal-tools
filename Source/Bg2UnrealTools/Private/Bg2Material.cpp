
#include "Bg2Material.h"

#include "CoreMinimal.h"
#include "ImageLoader.h"
#include "EngineMinimal.h"
#include "JsonUtilities.h"
#include "ConstructorHelpers.h"
#include "Misc/Paths.h"
#include "Bg2UnrealTools.h"

class MaterialParser {
public:
	enum ValueType {
		VT_Null = 0,
		VT_Texture,
		VT_Color,
		VT_Vector3,
		VT_Vector2,
		VT_Scalar,
		VT_String,
		VT_Boolean,
	};

	MaterialParser(UObject * Outer, const TSharedPtr<FJsonObject> & obj, const FString & basePath)
		:mOuter(Outer), mJsonObject(obj), mBasePath(basePath) {}

	bool IsPBR()
	{
		FString materialClass;
		if (mJsonObject->TryGetStringField("class", materialClass))
		{
			return materialClass == "PBRMaterial";
		}
		else {
			return false;
		}
	}

	ValueType GetString(const FString & paramName)
	{
		mResultString = "";
		if (mJsonObject->TryGetStringField(paramName, mResultString))
		{
			return VT_String;
		}
		else {
			return VT_Null;
		}
	}

	ValueType GetBoolean(const FString& paramName)
	{
		if (mJsonObject->TryGetBoolField(paramName, mBoolValue))
		{
			return VT_Boolean;
		}
		else
		{
			return VT_Null;
		}
	}

	ValueType GetColorOrTexture(const FString & paramName)
	{
		mResultTexture = nullptr;
		mResultColor = FLinearColor(0, 0, 0, 0);
		FString texture;
		const TArray<TSharedPtr<FJsonValue>> * color;
		
		if (mJsonObject->TryGetArrayField(paramName, color))
		{
			float components[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			for (int32 componentIndex = 0; componentIndex < color->Num() && componentIndex < 4; ++componentIndex)
			{
				components[componentIndex] = static_cast<float>((*color)[componentIndex]->AsNumber());
			}
			mResultColor = FLinearColor(components[0], components[1], components[2], components[3]);
			return VT_Color;
		}
		else if (mJsonObject->TryGetStringField(paramName, texture))
		{
			FString fullPath = FPaths::Combine(mBasePath, texture);
			mResultTexture = UImageLoader::LoadImageFromDisk(mOuter, fullPath);
			return mResultTexture != nullptr ? VT_Texture : VT_Null;
		}
		else
		{
			return VT_Null;
		}
	}

	ValueType GetScalarOrTexture(const FString & paramName)
	{
		mResultTexture = nullptr;
		mResultScalar = 0.0f;
		FString texture;
		double scalarValue;
		if (mJsonObject->TryGetNumberField(paramName, scalarValue))
		{
			mResultScalar = static_cast<float>(scalarValue);
			return VT_Scalar;
		}
		else if (mJsonObject->TryGetStringField(paramName, texture))
		{
			FString fullPath = FPaths::Combine(mBasePath, texture);
			mResultTexture = UImageLoader::LoadImageFromDisk(mOuter, fullPath);
			return mResultTexture != nullptr ? VT_Texture : VT_Null;
		}
		else
		{
			return VT_Null;
		}
	}

	ValueType GetTexture(const FString & paramName)
	{
		mResultTexture = nullptr;
		FString texture;
		if (mJsonObject->TryGetStringField(paramName, texture))
		{
			FString fullPath = FPaths::Combine(mBasePath, texture);
			mResultTexture = UImageLoader::LoadImageFromDisk(mOuter, fullPath);
			return mResultTexture != nullptr ? VT_Texture : VT_Null;
		}
		else
		{
			return VT_Null;
		}
	}

	ValueType GetScalar(const FString & paramName)
	{
		double result;
		if (mJsonObject->TryGetNumberField(paramName, result))
		{
			mResultScalar = static_cast<float>(result);
			return VT_Scalar;
		}
		else
		{
			return VT_Null;
		}
	}

	ValueType GetVector2(const FString & paramName)
	{
		const TArray<TSharedPtr<FJsonValue>> * result;
		if (mJsonObject->TryGetArrayField(paramName, result))
		{
			float components[] = { 0.0f, 0.0f };
			for (int32 componentIndex = 0; componentIndex < result->Num() && componentIndex < 4; ++componentIndex)
			{
				components[componentIndex] = static_cast<float>((*result)[componentIndex]->AsNumber());
			}
			mResultVector2 = FVector2D(components[0], components[1]);
			return VT_Vector2;
		}
		else
		{
			return VT_Null;
		}
	}

	inline const FString & GetResultString() const { return mResultString; }
	inline UTexture2D * GetResultTexture() { return mResultTexture; }
	inline const FLinearColor & GetResultColor() const { return mResultColor; }
	inline const FVector2D & GetResultVector2D() const { return mResultVector2; }
	inline float GetResultScalar() const { return mResultScalar; }
	inline bool GetResultBoolean() const { return mBoolValue; }

protected:
	UObject * mOuter;
	const TSharedPtr<FJsonObject> mJsonObject;
	FString mBasePath;

	// Result attributes
	FString mResultString = "";
	UTexture2D * mResultTexture = nullptr;
	FLinearColor mResultColor;
	FVector2D mResultVector2;
	float mResultScalar = 0.0f;
	bool mBoolValue = false;
};

UMaterialInstanceDynamic * UBg2Material::LoadMaterialWithJsonString(UMaterial * BaseMaterial, UObject * Outer, const FString & JsonString, const FString & BasePath, FMaterialProperties& MatProp)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		return LoadMaterialWithJsonObject(BaseMaterial, Outer, JsonObject, BasePath, MatProp);
	}
	else
	{
		return nullptr;
	}
}

UMaterialInstanceDynamic * UBg2Material::LoadMaterialWithJsonObject(UMaterial * BaseMaterial, UObject * Outer, const TSharedPtr<FJsonObject> & JsonObject, const FString & BasePath, FMaterialProperties& MatProp)
{
	UMaterialInstanceDynamic * result = nullptr;

	result = UMaterialInstanceDynamic::Create(BaseMaterial, Outer);
	MaterialParser parser(Outer, JsonObject, BasePath);
	FString matName = "-";
	if (parser.GetString("name"))
	{
		matName = parser.GetResultString();
	}

	if (parser.IsPBR())
	{
		switch (parser.GetColorOrTexture("diffuse"))
		{
		case MaterialParser::VT_Texture:
		{
			// Diffuse texture
			result->SetTextureParameterValue(TEXT("DiffuseTexture"), parser.GetResultTexture());
			if (parser.GetScalar("diffuseUV") == MaterialParser::VT_Scalar)
			{
				result->SetScalarParameterValue(TEXT("DiffuseUV"), parser.GetResultScalar());
			}

			if (parser.GetVector2("diffuseScale") == MaterialParser::VT_Vector2)
			{
				result->SetScalarParameterValue(TEXT("DiffuseScaleU"), parser.GetResultVector2D().X);
				result->SetScalarParameterValue(TEXT("DiffuseScaleV"), parser.GetResultVector2D().Y);
			}
			break;
		}
		case MaterialParser::VT_Color:
			// Diffuse color
			result->SetVectorParameterValue(TEXT("DiffuseColor"), parser.GetResultColor());
			break;
		}


		if (parser.GetTexture("normal") == MaterialParser::VT_Texture)
		{
			// Normal map
			result->SetTextureParameterValue(TEXT("NormalTexture"), parser.GetResultTexture());
			if (parser.GetScalar("normalUV") == MaterialParser::VT_Scalar)
			{
				result->SetScalarParameterValue(TEXT("NormalUV"), parser.GetResultScalar());
			}
			if (parser.GetVector2("normalScale") == MaterialParser::VT_Vector2)
			{
				result->SetScalarParameterValue(TEXT("NormalScaleU"), parser.GetResultVector2D().X);
				result->SetScalarParameterValue(TEXT("NormalScaleV"), parser.GetResultVector2D().Y);
			}
		}

		switch (parser.GetScalarOrTexture("roughness"))
		{
		case MaterialParser::VT_Texture:
		{
			result->SetTextureParameterValue(TEXT("RoughnessTexture"), parser.GetResultTexture());
			if (parser.GetScalar("roughnessUV") == MaterialParser::VT_Scalar)
			{
				result->SetScalarParameterValue(TEXT("RoughnessUV"), parser.GetResultScalar());
			}
			if (parser.GetVector2("roughnessScale") == MaterialParser::VT_Vector2)
			{
				result->SetScalarParameterValue(TEXT("RoughnessScaleU"), parser.GetResultVector2D().X);
				result->SetScalarParameterValue(TEXT("RoughnessScaleV"), parser.GetResultVector2D().Y);
			}
			break;
		}
		case MaterialParser::VT_Scalar:
			result->SetScalarParameterValue(TEXT("Roughness"), parser.GetResultScalar());
			break;
		}

		switch (parser.GetScalarOrTexture("metallic"))
		{
		case MaterialParser::VT_Texture:
		{
			result->SetTextureParameterValue(TEXT("MetallicTexture"), parser.GetResultTexture());
			if (parser.GetScalar("metallicUV") == MaterialParser::VT_Scalar)
			{
				result->SetScalarParameterValue(TEXT("MetallicUV"), parser.GetResultScalar());
			}
			if (parser.GetVector2("MetallicScale") == MaterialParser::VT_Vector2)
			{
				result->SetScalarParameterValue(TEXT("MetallicScaleU"), parser.GetResultVector2D().X);
				result->SetScalarParameterValue(TEXT("MetallicScaleV"), parser.GetResultVector2D().Y);
			}
			break;
		}
		case MaterialParser::VT_Scalar:
			result->SetScalarParameterValue(TEXT("Metallic"), parser.GetResultScalar());
			break;
		}

		switch (parser.GetScalarOrTexture("height"))
		{
		case MaterialParser::VT_Texture:
		{
			result->SetTextureParameterValue(TEXT("HeightTexture"), parser.GetResultTexture());
			if (parser.GetScalar("heightUV") == MaterialParser::VT_Scalar)
			{
				result->SetScalarParameterValue(TEXT("HeightUV"), parser.GetResultScalar());
			}
			if (parser.GetVector2("heightScale") == MaterialParser::VT_Vector2)
			{
				result->SetScalarParameterValue(TEXT("HeightScaleU"), parser.GetResultVector2D().X);
				result->SetScalarParameterValue(TEXT("HeightScaleV"), parser.GetResultVector2D().Y);
			}
		}
		}

		if (parser.GetBoolean("visible") == MaterialParser::VT_Boolean)
		{
			MatProp.Visible = parser.GetResultBoolean();
		}
		else
		{
			MatProp.Visible = true;
		}
	}
	else
	{
		UE_LOG(Bg2Tools, Warning, TEXT("Could not load bg2 material: the material is not a PBR material. Material name: %s"), *matName);
	}

	
	return result;
}

void UBg2Material::GetExternalResources(const TSharedPtr<FJsonObject> & JsonObject, TArray<FString> & Result)
{
	TArray<FString> resourceItems = {
		"diffuse",
		"metallic",
		"roughness",
		"height",
		"normal"
	};

	for (auto i = 0; i < resourceItems.Num(); ++i)
	{
		const FString & key = resourceItems[i];
		FString value;
		double numberValue;
		if (!JsonObject->TryGetNumberField(key, numberValue) && JsonObject->TryGetStringField(key, value))
		{
			Result.Add(value);
		}
	}
}



