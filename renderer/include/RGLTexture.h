#pragma once
#include "RBaseTexture.h"

#ifdef RND_GL
namespace RAPI
{
	class RGLTexture : public RBaseTexture
	{
	public:
		RGLTexture();
		~RGLTexture();

		/**
         * Creates the textureobjects for the given API
         */
		bool CreateTextureAPI(const void* textureData,
							  std::vector<void*> mipData = std::vector<void*>());

		/**
        * Loads a texture from the given file
        */
		bool CreateFromFileAPI(const std::string& file);

		/**
         * Updates a subresource with default usage
         */
		bool UpdateSubresourceAPI(void* data, int mipLevel = 0, int arrayIndex = 0);

		/**
        * Maps the texture for update. Only possible with the right CPU-Acces and usage-flags.
        */
		bool MapAPI(void** dataOut, int subresource);

		/**
        * Unmaps the texture
        */
		bool UnmapAPI(int subresource);

		/**
         * Deletes all resources this holds but keeps the object around.
         * Recreate the texture by calling CreateTexture
         */
		void DeallocateAPI();

		/**
         * Tries to save this texture to a file. Only works for RGBA8-Format.
         */
		bool SaveToFileAPI(const std::string& file){return true;}

		/**
		 * Returns the texture-object owned by this texture
		 */
		GLuint GetTextureObjectAPI() { return TextureObject; }
	private:

		/**
		* Cleans the created API-Resources
		*/
		void CleanAPI();
		/**
		 * Texture handle
		 */
		GLuint TextureObject;
		GLenum GLTextureFormat;
		GLenum GLTextureType;
	};
}
#endif