#include <string.h>
#include "pch.h"
#include "RTextureAtlas.h"
#include "REngine.h"
#include "RResourceCache.h"
#include "RTexture.h"
#include "Types.h"

// Maximum dimensions of the atlas
const size_t ATLAS_MAX_PIXEL_SIZE = 16384;

namespace RAPI
{

	RTextureAtlas::RTextureAtlas()
	{
		Atlas = nullptr;
	}

	RTextureAtlas::~RTextureAtlas()
	{
		REngine::ResourceCache->DeleteResource(Atlas);
	}

/** Stores a copy of the texturedata you give to this for later processing.
	Returns the index of this texture so you can query the UVs later.
	The given size must be the same for all textures. */
	void RTextureAtlas::StoreTexture(byte *data, size_t dataSize, void *userptr)
	{
		if (Atlas)
			return;

		StoredTextures[userptr].resize(dataSize / 4);

		// Make a copy of the data and push to internal storage
		memcpy(&StoredTextures[userptr][0], data, dataSize);
	}

/** Constructs the textureatlas out of all the given textures */
	bool RTextureAtlas::Construct(RInt2 singleTextureDimensions)
	{
		if (Atlas) // Already created?
			return false;

		// Compute stride and amount of our textures
		size_t fullstride = singleTextureDimensions.x * singleTextureDimensions.y * 4;
		size_t numTextures = StoredTextures.size();



		// Compute UVs and actual texture size
		RInt2 texturePos = RInt2(0, 0);
		RInt2 textureSize = singleTextureDimensions;
		for (auto &t : StoredTextures) {
			// Check if we need a bigger size
			if (texturePos.x == textureSize.x && texturePos.y == textureSize.y - singleTextureDimensions.y) {
				// Append on the right
				if (textureSize.x == textureSize.y) {
					textureSize.x *= 2;
					texturePos.y = 0;
				}
				else // Append on the bottom
				{
					textureSize.y *= 2;
					texturePos.x = 0;
					texturePos.y += singleTextureDimensions.y;
				}
			}
			else if (texturePos.x == textureSize.x) {
				// Simply move one line down
				if (textureSize.x == textureSize.y)
					texturePos.x = 0;
				else
					texturePos.x = textureSize.x / 2;

				texturePos.y += singleTextureDimensions.y;
			}

			// Store the UV for the texture
			TextureUVs[t.first] = std::make_pair(RFloat2((float)texturePos.x, (float)texturePos.y), RFloat2(0, 0));

			// Goto next position
			texturePos.x += singleTextureDimensions.x;
		}

		// Create texture-memory
		size_t actualSize = textureSize.x * textureSize.y * 4;
		uint32_t *mem = new uint32_t[actualSize / 4];

		// Copy memory of each texture to it's place
		for (auto &t : TextureUVs) {
			int sx = (int)t.second.first.x;
			int sy = (int)t.second.first.y;
			auto &st = StoredTextures[t.first];
			for (int y = 0; y < singleTextureDimensions.y; y++) {
				for (int x = 0; x < singleTextureDimensions.x; x++) {
					mem[sx + x + textureSize.x * (sy + y)] = st[x + singleTextureDimensions.x * y];
				}
			}
		}

		// Normalize UVs
		for (auto &p : TextureUVs) {
			p.second.first.x /= textureSize.x;
			p.second.first.y /= textureSize.y;

			p.second.second.x = (float)textureSize.x / singleTextureDimensions.x;
			p.second.second.y = (float)textureSize.y / singleTextureDimensions.y;
		}

		// Create GPU-Resource
		Atlas = REngine::ResourceCache->CreateResource<RTexture>();
		Atlas->CreateTexture(mem, (unsigned int)actualSize, textureSize, 1);

		// Clear memory
		StoredTextures.clear();

		delete[] mem;

		return true;
	}
}