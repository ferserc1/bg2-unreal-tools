
#include "ImageLoader.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"
#include "RenderUtils.h"
#include "Engine/Texture2D.h"
#include "Bg2UnrealTools.h"


IImageWrapperModule* ImageWrapperModule = nullptr;

UTexture2D * UImageLoader::LoadImageFromDisk(UObject * Outer, const FString & ImagePath)
{
	Bg2UnrealToolsImpl * unrealToolsModule = FModuleManager::Get().GetModulePtr<Bg2UnrealToolsImpl>("Bg2UnrealTools");
	UImageCache * imageCache = unrealToolsModule->GetImageCache(Outer);
	if (imageCache->FindTexture(ImagePath))
	{
		UE_LOG(Bg2Tools, Warning, TEXT("Image present in cache. Skip load"));
		return imageCache->GetTexture(ImagePath);
	}
	UE_LOG(Bg2Tools, Warning, TEXT("Image not present in cache. Loading texture %s"), *ImagePath);

	if (!ImageWrapperModule)
	{
		ImageWrapperModule = &FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
	}

	// Check if the file exists first
	if (!FPaths::FileExists(ImagePath))
	{
		//UIL_LOG(Error, TEXT("File not found: %s"), *ImagePath);
		return nullptr;
	}

	// Load the compressed byte data from the file
	TArray<uint8> FileData;
	if (!FFileHelper::LoadFileToArray(FileData, *ImagePath))
	{
		//UIL_LOG(Error, TEXT("Failed to load file: %s"), *ImagePath);
		return nullptr;
	}

	// Detect the image type using the ImageWrapper module
	EImageFormat ImageFormat = ImageWrapperModule->DetectImageFormat(FileData.GetData(), FileData.Num());
	if (ImageFormat == EImageFormat::Invalid)
	{
		//UIL_LOG(Error, TEXT("Unrecognized image file format: %s"), *ImagePath);
		return nullptr;
	}

	// Create an image wrapper for the detected image format
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule->CreateImageWrapper(ImageFormat);
	if (!ImageWrapper.IsValid())
	{
		//UIL_LOG(Error, TEXT("Failed to create image wrapper for file: %s"), *ImagePath);
		return nullptr;
	}

	// Decompress the image data
	TArray<uint8> RawData;
	ImageWrapper->SetCompressed(FileData.GetData(), FileData.Num());
	ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData);
	if (RawData.Num() == 0)
	{
		//UIL_LOG(Error, TEXT("Failed to decompress image file: %s"), *ImagePath);
		return nullptr;
	}

	// Create the texture and upload the uncompressed image data
	FString TextureBaseName = TEXT("Texture_") + FPaths::GetBaseFilename(ImagePath);
	auto texture = CreateTexture(Outer, RawData, ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), EPixelFormat::PF_B8G8R8A8, FName(*TextureBaseName));
	imageCache->AddTexture(ImagePath, texture);
	return texture;
}

UTexture2D * UImageLoader::CreateTexture(UObject * Outer, const TArray<uint8> & PixelData, int32 InSizeX, int32 InSizeY, EPixelFormat InFormat, FName BaseName)
{
	// Shamelessly copied from UTexture2D::CreateTransient with a few modifications
	if (InSizeX <= 0 || InSizeY <= 0 ||
		(InSizeX % GPixelFormats[InFormat].BlockSizeX) != 0 ||
		(InSizeY % GPixelFormats[InFormat].BlockSizeY) != 0)
	{
		//UIL_LOG(Warning, TEXT("Invalid parameters specified for UImageLoader::CreateTexture()"));
		return nullptr;
	}

	// Most important difference with UTexture2D::CreateTransient: we provide the new texture with a name and an owner
	FName TextureName = MakeUniqueObjectName(Outer, UTexture2D::StaticClass(), BaseName);
	UTexture2D * NewTexture = NewObject<UTexture2D>(Outer, TextureName, RF_Transient);

	auto platformData = new FTexturePlatformData();
	platformData->SizeX = InSizeX;
	platformData->SizeY = InSizeY;
	platformData->PixelFormat = InFormat;
	NewTexture->SetPlatformData(platformData);
	NewTexture->Filter = TF_Trilinear;

	// Allocate first mipmap and upload the pixel data
	int32 NumBlocksX = InSizeX / GPixelFormats[InFormat].BlockSizeX;
	int32 NumBlocksY = InSizeY / GPixelFormats[InFormat].BlockSizeY;
	FTexture2DMipMap * Mip = new FTexture2DMipMap();
	platformData->Mips.Add(Mip);
	Mip->SizeX = InSizeX;
	Mip->SizeY = InSizeY;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	void * TextureData = Mip->BulkData.Realloc(NumBlocksX * NumBlocksY * GPixelFormats[InFormat].BlockBytes);
	FMemory::Memcpy(TextureData, PixelData.GetData(), PixelData.Num());
	Mip->BulkData.Unlock();

		// Mipmap generation
		int mipsToAdd = 3;
		TArray<uint8_t> _mipRGBAs;
		TArray<uint8_t> _mipRGBBs;

		auto* priorData = (const uint8*)platformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		int priorWidth = platformData->Mips[0].SizeX;
		int priorHeight = platformData->Mips[0].SizeY;
		const int BYTES_PER_PIXEL = 4;

		while (mipsToAdd > 0)
		{
			auto* mipRGBAs = mipsToAdd & 1 ? &_mipRGBAs : &_mipRGBBs;

			int mipWidth = priorWidth >> 1;
			int mipHeight = priorHeight >> 1;
			if ((mipWidth == 0) || (mipHeight == 0))
			{
				break;
			}

			mipRGBAs->Reset();
			mipRGBAs->AddUninitialized(mipWidth * mipHeight * BYTES_PER_PIXEL);
			int dataPerRow = priorWidth * BYTES_PER_PIXEL;

			auto* dataOut = mipRGBAs->GetData();
			for (int y = 0; y < mipHeight; ++y)
			{
				auto* dataInRow0 = priorData + (dataPerRow * y * 2);
				auto* dataInRow1 = dataInRow0 + dataPerRow;
				for (int x = 0; x < mipWidth; x++)
				{
					int totalB = *dataInRow0++;
					int totalG = *dataInRow0++;
					int totalR = *dataInRow0++;
					int totalA = *dataInRow0++;
					totalB += *dataInRow0++;
					totalG += *dataInRow0++;
					totalR += *dataInRow0++;
					totalA += *dataInRow0++;

					totalB += *dataInRow1++;
					totalG += *dataInRow1++;
					totalR += *dataInRow1++;
					totalA += *dataInRow1++;
					totalB += *dataInRow1++;
					totalG += *dataInRow1++;
					totalR += *dataInRow1++;
					totalA += *dataInRow1++;

					totalB >>= 2;
					totalG >>= 2;
					totalR >>= 2;
					totalA >>= 2;

					*dataOut++ = (uint8)totalB;
					*dataOut++ = (uint8)totalG;
					*dataOut++ = (uint8)totalR;
					*dataOut++ = (uint8)totalA;
				}
				dataInRow0 += priorWidth * 2;
				dataInRow1 += priorWidth * 2;
			}
			Mip = new FTexture2DMipMap();
			platformData->Mips.Add(Mip);
			Mip->SizeX = mipWidth;
			Mip->SizeY = mipHeight;
			Mip->BulkData.Lock(LOCK_READ_WRITE);
			void* mipData = Mip->BulkData.Realloc(mipRGBAs->Num());
			FMemory::Memcpy(mipData, mipRGBAs->GetData(), mipRGBAs->Num());
			Mip->BulkData.Unlock();

			priorData = mipRGBAs->GetData();
			priorWidth = mipWidth;
			priorHeight = mipHeight;
			--mipsToAdd;
		}
		platformData->Mips[0].BulkData.Unlock();

	NewTexture->UpdateResource();
	return NewTexture;
}
