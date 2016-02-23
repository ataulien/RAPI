#pragma once
#include "Types.h"
#include "RResource.h"

namespace RAPI
{
/**
 * base class for textures. Holds generic data about textures
 */
	class RBaseTexture : public RResource
	{
	public:

		RBaseTexture();

		virtual ~RBaseTexture();

		/**
         *Returns the RowPitch-Bytes
         */
		uint32_t ComputeRowPitchBytes(int mip);

		/**
          * Returns the size of the texture in bytes
          */
		uint32_t ComputeSizeInBytes(int mip);

		/**
          * Returns the size of the texture in bytes
          */
		static uint32_t ComputeSizeInBytes(int mip, const RInt2 &resolution, ETextureFormat format);

		static uint32_t ComputeRowPitchBytes(int mip, const RInt2 &resolution, ETextureFormat format);

		/** Returns the size of a DDS-Image in bytes */
		static unsigned int GetDDSStorageRequirements(unsigned int width, unsigned int height, bool dxt1);

		/**
         * Getters
         */
		ETextureFormat GetTextureFormat()
		{ return TextureFormat; }

		uint32_t GetSizeInBytes()
		{ return SizeInBytes; }

		RInt2 GetResolution()
		{ return Resolution; }

		uint32_t GetNumMipLevels()
		{ return NumMipLevels; }

		EBindFlags GetBindFlags()
		{ return BindFlags; }

		EUsageFlags GetUsageFlags()
		{ return UsageFlags; }

	protected:
		// Size of the whole texture in bytes
		uint32_t SizeInBytes;

		// Format of the texture data
		ETextureFormat TextureFormat;

		// Wheather this texture is dynamic or not
		bool IsDynamic;

		// Size of the textures highest miplevel in pixels
		RInt2 Resolution;

		// Number if mip-levels we have
		uint32_t NumMipLevels;

		// Bind flags
		EBindFlags BindFlags;

		// Usage
		EUsageFlags UsageFlags;

		// True if everything was created properly
		bool IsFullyInitialized;

		// Number of texture we have in a texture array
		unsigned int ArraySize;

		// Whether the memory this texture is being loaded from contains a dds header
		bool MemoryContainsDDSHeader;
	};
}