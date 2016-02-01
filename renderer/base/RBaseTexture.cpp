#include <math.h>
#include "pch.h"
#include "RBaseTexture.h"

using namespace RAPI;

RBaseTexture::RBaseTexture()
{
	SizeInBytes = 0;
	TextureFormat = ETextureFormat::TF_R8G8B8A8;
	NumMipLevels = 0;
	BindFlags = EBindFlags::B_SHADER_RESOURCE;
	UsageFlags = EUsageFlags::U_DEFAULT;
	IsFullyInitialized = false;
}


RBaseTexture::~RBaseTexture()
{
}

/** Returns the size of a DDS-Image in bytes */
unsigned int GetDDSStorageRequirements(unsigned int width, unsigned int height, bool dxt1)
{
	// compute the storage requirements
	int blockcount = ( ( width + 3 )/4 ) * ( ( height + 3 )/4 );
	int blocksize = dxt1 ? 8 : 16;
	return blockcount*blocksize;
}

/** Returns the RowPitch-Size of a DDS-Image */
unsigned int GetDDSRowPitchSize(unsigned int width, bool dxt1)
{
	if(dxt1)
		return std::max((unsigned int)1, ((width+3)/4)) * 8;
	else
		return std::max((unsigned int)1, ((width+3)/4)) * 16;
}

uint32_t RBaseTexture::ComputeSizeInBytes(int mip, const int2 & resolution, ETextureFormat format)
{
	EFormat f = (EFormat)format;
	int px = (int)std::max(1.0f, (float)floor(resolution.x / pow(2.0f, mip)));
	int py = (int)std::max(1.0f, (float)floor(resolution.y / pow(2.0f, mip)));
	//int px = TextureSize.x;
	//int py = TextureSize.y;

	if (f == FORMAT_BC1_UNORM || f == FORMAT_BC2_UNORM ||
		f == FORMAT_BC3_UNORM)
	{
		return GetDDSStorageRequirements(px, py, f == FORMAT_BC1_UNORM);
	}
	else // Use R8G8B8A8
	{
		return px * py * 4;
	}
}

uint32_t RBaseTexture::ComputeRowPitchBytes(int mip, const int2 & resolution, ETextureFormat format)
{
	EFormat f = (EFormat)format;
	int px = (int)std::max(1.0f, (float)floor(resolution.x / pow(2.0f, mip)));
	int py = (int)std::max(1.0f, (float)floor(resolution.y / pow(2.0f, mip)));
	//int px = TextureSize.x;
	//int py = TextureSize.y;

	if (f == FORMAT_BC1_UNORM || f == FORMAT_BC2_UNORM ||
		f == FORMAT_BC3_UNORM)
	{
		return GetDDSRowPitchSize(px, f == FORMAT_BC1_UNORM);
	}
	else // Use R8G8B8A8
	{
		return px * 4;
	}
}

/** Returns the RowPitch-Bytes */
uint32_t RBaseTexture::ComputeRowPitchBytes(int mip)
{
	return RBaseTexture::ComputeRowPitchBytes(mip, Resolution, TextureFormat);
}

/** Returns the size of the texture in bytes */
uint32_t RBaseTexture::ComputeSizeInBytes(int mip)
{
	return RBaseTexture::ComputeSizeInBytes(mip, Resolution, TextureFormat);
}
