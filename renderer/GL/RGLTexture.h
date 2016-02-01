#pragma once
#include "RBaseTexture.h"

#ifdef RND_GL
namespace RAPI
{
	class RGLTexture : public RBaseTexture
	{
	public:
		/**
         * Creates the textureobjects for the given API
         */
		bool CreateTextureAPI(const void* textureData,
							  std::vector<void*> mipData = std::vector<void*>()){return true;}

		/**
        * Loads a texture from the given file
        */
		bool CreateFromFileAPI(const std::string& file){return true;}

		/**
         * Updates a subresource with default usage
         */
		bool UpdateSubresourceAPI(void* data, int mipLevel = 0, int arrayIndex = 0){return true;}

		/**
        * Maps the texture for update. Only possible with the right CPU-Acces and usage-flags.
        */
		bool MapAPI(void** dataOut, int subresource){return true;}

		/**
        * Unmaps the texture
        */
		bool UnmapAPI(int subresource){return true;}

		/**
         * Deletes all resources this holds but keeps the object around.
         * Recreate the texture by calling CreateTexture
         */
		void DeallocateAPI(){}

		/**
         * Tries to save this texture to a file. Only works for RGBA8-Format.
         */
		bool SaveToFileAPI(const std::string& file){return true;}
	};
}
#endif