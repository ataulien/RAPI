#pragma once
#include "pch.h"
#include "Types.h"

#ifdef RND_D3D11
#include "RD3D11Texture.h"
#define RTEXTUREBASE_API RD3D11Texture
#elif defined(RND_NULL)
#include "RNULLTexture.h"
#define RTEXTUREBASE_API RNULLTexture
#elif defined(RND_GL)
#include "RGLTexture.h"
#define RTEXTUREBASE_API RGLTexture
#endif

namespace RAPI
{
	class RTexture :
			public RTEXTUREBASE_API
	{
	public:
		RTexture();

		~RTexture();

		/**
		 * Creates the textureobject and initializes the data
		 */
		bool CreateTexture(const void *textureData,
						   uint32_t sizeInBytes,
						   int2 resolution,
						   unsigned int numMipLevels = 10,
						   ETextureFormat textureFormat = ETextureFormat::TF_R8G8B8A8,
						   EBindFlags bindFlags = EBindFlags::B_SHADER_RESOURCE,
						   EUsageFlags usageFlags = EUsageFlags::U_DEFAULT,
						   unsigned int arraySize = 1,
						   std::vector<void *> mipData = std::vector<void *>());

		/**
		 * Loads a texture from the given file
		 */
		bool CreateFromFile(const std::string &file);


		/**
		 * Maps the texture for update. Only possible with the right CPU-Acces and usage-flags.
		 */
		bool Map(void **dataOut, int subresource = 0);

		/**
		 * Unmaps the texture
		 */
		bool Unmap(int subresource = 0);

		/**
		 * Updates a subresource with default usage
		 */
		bool UpdateSubresource(void *data, int mipLevel = 0, int arrayIndex = 0);

		/**
		 * Deletes all resources this holds but keeps the object around.
		 * Recreate the texture by calling CreateTexture
		 */
		void Deallocate();

		/**
		 * Returns wheather this texture is currently initialized and ready for use
		 */
		bool IsInitialized();
	};

}