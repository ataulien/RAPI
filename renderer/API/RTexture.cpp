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
    // Assign data
    TextureFormat = textureFormat;
    SizeInBytes = sizeInBytes;
    Resolution = resolution;
    NumMipLevels = numMipLevels;
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