#pragma once
#include "RResource.h"

namespace RAPI
{
	class RTexture;

/** Simple texture atlas. No Mipmap-Support yet. Only supports RGBA8-Textures at the moment. 
	All textures must be of the same size. */
	class RTextureAtlas : public RResource
	{
	public:
		RTextureAtlas();

		~RTextureAtlas();

		/** Stores a copy of the texturedata you give to this for later processing.
			The given size must be the same for all textures.
			userptr can be used to identify the texture later*/
		void StoreTexture(byte *data, size_t dataSize, void *userptr);

		/** Constructs the textureatlas out of all the given textures */
		bool Construct(RInt2 singleTextureDimensions);

		/** Returns the atlas' texture object */
		RTexture *GetTexture()
		{ return Atlas; }

		/** Returns the modified UVs for the given texture */
		const std::pair<RFloat2, RFloat2> &GetModifiedUV(void *userptr)
		{ return TextureUVs[userptr]; }

	private:

		// Vector of pointers to the texture-data we stored
		std::map<void *, std::vector<RDWORD>> StoredTextures;

		// Min/Max UVs for all registered textures
		std::map<void *, std::pair<RFloat2, RFloat2>> TextureUVs;

		// Constructed atlas
		RTexture *Atlas;
	};

}