#include "pch.h"
#include "RTexture.h"

using namespace RAPI;

RTexture::RTexture()
{
}


RTexture::~RTexture()
{
}

/**
* Creates the textureobject and initializes the data 
*/
bool RTexture::CreateTexture(const void *textureData,
                             uint32_t sizeInBytes,
                             RInt2 resolution,
                             unsigned int numMipLevels,
                             ETextureFormat textureFormat,
                             EBindFlags bindFlags,
                             EUsageFlags usageFlags,
                             unsigned int arraySize,
                             std::vector<void *> mipData)
{
	// Get information from DDS-Header
	if(textureFormat == ETextureFormat::TF_FORMAT_UNKNOWN_DXT)
	{
		SetFromDDSHeader((DDSURFACEDESC2*)(((char*)textureData) + 4));
		MemoryContainsDDSHeader = true;
	}
	else
	{	
		TextureFormat = textureFormat;
		SizeInBytes = sizeInBytes;
		Resolution = resolution;
		NumMipLevels = numMipLevels;
	}

	// Assign data  
    BindFlags = bindFlags;
    UsageFlags = usageFlags;
    ArraySize = arraySize;

    // Create API-Specific resources
    if (!CreateTextureAPI(textureData, mipData))
        return false;

    return true;
}

/**
* Loads a texture from the given file
*/
bool RTexture::CreateFromFile(const std::string &file)
{
    return CreateFromFileAPI(file);
}

/**
 * Updates a subresource with default usage
 */
bool RTexture::UpdateSubresource(void *data, int mipLevel, int arrayIndex)
{
    return UpdateSubresourceAPI(data, mipLevel, arrayIndex);
}

/**
* Maps the texture for update. Only possible with the right CPU-Acces and usage-flags.
*/
bool RTexture::Map(void **dataOut, int subresource)
{
    if (!MapAPI(dataOut, subresource))
        return false;

    return true;
}

/**
* Unmaps the texture
*/
bool RTexture::Unmap(int subresource)
{
    if (!UnmapAPI(subresource))
        return false;

    return true;
}

/**
 * Deletes all resources this holds but keeps the object around.
 * Recreate the texture by calling CreateTexture
 */
void RTexture::Deallocate()
{
    DeallocateAPI();
}

/**
 * Returns wheather this texture is currently initialized and ready for use
 */
bool RTexture::IsInitialized()
{
    return IsFullyInitialized;
}

/** 
* Sets file information from a given DDS-Header
*/
bool RTexture::SetFromDDSHeader(const RAPI::DDSURFACEDESC2* desc)
{
	// Find DXT-Format
	if((desc->ddpfPixelFormat.dwFlags & DDPF_FOURCC) == DDPF_FOURCC)
	{
		switch(desc->ddpfPixelFormat.dwFourCC)
		{
		case RAPI_FOURCC_DXT1:
			TextureFormat = ETextureFormat::TF_DXT1;
			break;
		case RAPI_FOURCC_DXT2:
		case RAPI_FOURCC_DXT3:
			TextureFormat = ETextureFormat::TF_DXT3;
			break;
		case RAPI_FOURCC_DXT4:
		case RAPI_FOURCC_DXT5:
			TextureFormat = ETextureFormat::TF_DXT5;
			break;
		}
	}else
	{
		return false;
	}

	Resolution = RInt2(desc->dwWidth, desc->dwHeight);

	// Assign data
	SizeInBytes = GetDDSStorageRequirements(Resolution.x, Resolution.y, TextureFormat == ETextureFormat::TF_DXT1);
	//NumMipLevels = desc->dwMipMapCount;
	NumMipLevels = 1;
	return true;
}