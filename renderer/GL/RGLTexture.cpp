#include "pch.h"
#include "RGLTexture.h"

#ifdef RND_GL
#include "REngine.h"
#include "Logger.h"
#include "lib/nv_dds/nv_dds.h"
#include <sstream>
#include <stdexcept>

using namespace RAPI;

RGLTexture::RGLTexture()
{
	TextureObject = 0;
}


RGLTexture::~RGLTexture()
{
	CleanAPI();
}

/**
* Loads a texture from the given file
*/
bool RGLTexture::CreateFromFileAPI(const std::string& file)
{
	nv_dds::CDDSImage image;
	image.load(file);

	// Load texture
	if(!image.is_valid())
		return false;

	GLTextureFormat = image.get_format();

	// Create texture object
	glGenTextures(1, &TextureObject);
	glBindTexture(GL_TEXTURE_2D, TextureObject);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ); 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR ); 

	if(image.is_compressed())
	{
		glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, image.get_format(),
			image.get_width(), image.get_height(), 0, image.get_size(),
			image);

		for(int i = 0; i < image.get_num_mipmaps(); i++)
		{
			nv_dds::CSurface mipmap = image.get_mipmap(i);

			glCompressedTexImage2DARB(GL_TEXTURE_2D, i + 1, image.get_format(),
				mipmap.get_width(), mipmap.get_height(), 0, mipmap.get_size(),
				mipmap);
		}
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, image.get_components(), image.get_width(),
		    image.get_height(), 0, image.get_format(), GL_UNSIGNED_BYTE, image);
		
		for (int i = 0; i < image.get_num_mipmaps(); i++)
		{
		    glTexImage2D(GL_TEXTURE_2D, i+1, image.get_components(),
		        image.get_mipmap(i).get_width(), image.get_mipmap(i).get_height(),
		        0, image.get_format(), GL_UNSIGNED_BYTE, image.get_mipmap(i));
		}
	}

	return true;
}

/**
* Deletes all resources this holds but keeps the object around.
* Recreate the texture by calling CreateTexture
*/
void RGLTexture::DeallocateAPI()
{
	CleanAPI();
}

/**
* Creates the textureobjects for the given API 
*/
bool RGLTexture::CreateTextureAPI(const void* textureData,
	std::vector<void*> mipData)
{
	if(TextureFormat != TF_DXT1 && TextureFormat != TF_DXT3 && TextureFormat != TF_DXT5)
		return false; // TODO: Implement these!

	// TODO: Implement DepthStencil-Textures!

	size_t size = SizeInBytes;

	// Clean up first, so we can keep these texture-objects
	CleanAPI();

	// Restore size, as CleanAPI set it to 0
	SizeInBytes = size;

	// Load image
	nv_dds::CDDSImage image;

	std::string data((char*)textureData, SizeInBytes);
	std::istringstream mb(data);

	try{
		image.load(mb, false);
	}
	catch(std::runtime_error e)
	{
		LogError() << e.what();
		return false;
	}

	CheckGlError();

	if(!image.is_valid())
		return false;

	GLTextureFormat = image.get_format();
	GLTextureType = image.get_type();

	// Create texture object
	glGenTextures(1, &TextureObject);
	CheckGlError();

	glBindTexture(GL_TEXTURE_2D, TextureObject);
	CheckGlError();

	// Optional dds-header skip
	uint32_t skip = 0;
	if(MemoryContainsDDSHeader)
		skip = sizeof(uint32_t) + sizeof(DDSURFACEDESC2);

	if(image.is_compressed())
	{
		glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, image.get_format(),
			image.get_width(), image.get_height(), 0, image.get_size(),
			image.get_data());
		CheckGlError();

		for(int i = 0; i < image.get_num_mipmaps(); i++)
		{
			nv_dds::CSurface mipmap = image.get_mipmap(i);

			glCompressedTexImage2DARB(GL_TEXTURE_2D, i + 1, image.get_format(),
				mipmap.get_width(), mipmap.get_height(), 0, mipmap.get_size(),
				mipmap);
			CheckGlError();
		}
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, image.get_components(), image.get_width(),
			image.get_height(), 0, image.get_format(), GL_UNSIGNED_BYTE, image);
		CheckGlError();

		for (int i = 0; i < image.get_num_mipmaps(); i++)
		{
			glTexImage2D(GL_TEXTURE_2D, i+1, image.get_components(),
				image.get_mipmap(i).get_width(), image.get_mipmap(i).get_height(),
				0, image.get_format(), GL_UNSIGNED_BYTE, image.get_mipmap(i));
			CheckGlError();

		}
	}

	IsFullyInitialized = true;

	return true;
}

/**
* Updates a subresource with default usage
*/
bool RGLTexture::UpdateSubresourceAPI(void* data, int mipLevel, int arrayIndex)
{
	glBindTexture(GL_TEXTURE_2D, TextureObject);
	CheckGlError();

	glTexSubImage2D(GL_TEXTURE_2D, mipLevel, 0, 0, Resolution.x, Resolution.y, GLTextureFormat,
		GLTextureType, data);
	CheckGlError();
	return true;
}

/**
* Maps the texture for update. Only possible with the right CPU-Acces and usage-flags.
*/
bool RGLTexture::MapAPI(void ** dataOut, int subresource)
{
	// TODO: Implement
	*dataOut = nullptr;
	return false;
}

/**
* Unmaps the texture
*/
bool RGLTexture::UnmapAPI(int subresource)
{
	// TODO: Implement
	return false;
}

/**
* Cleans the created API-Resources
*/
void RGLTexture::CleanAPI()
{
	IsFullyInitialized = false;

	glDeleteTextures(1, &TextureObject);

	// Reset this so we know this texture isn't valid anymore
	SizeInBytes = 0;
}

#endif