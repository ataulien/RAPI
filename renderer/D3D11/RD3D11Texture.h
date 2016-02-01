#pragma once
#include "RBaseTexture.h"

namespace RAPI
{

#ifdef RND_D3D11

    class RD3D11Texture :
        public RBaseTexture
    {
    public:
        RD3D11Texture();
        ~RD3D11Texture();

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
         * Creates this texture directly from an API-Resource
         */
        bool CreateDirect(ID3D11Texture2D* texture,
            int2 resolution,
            unsigned int numMipLevels = 10,
            ETextureFormat textureFormat = ETextureFormat::TF_R8G8B8A8,
            EBindFlags bindFlags = EBindFlags::B_SHADER_RESOURCE,
            EUsageFlags usageFlags = EUsageFlags::U_DEFAULT);

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
        bool SaveToFileAPI(const std::string& file);

        /**
         * Getters
         */
        ID3D11Texture2D* GetTexture2D(){return Texture2D;}
        ID3D11ShaderResourceView* GetShaderResourceView(){return TextureSRV;}
        ID3D11ShaderResourceView*const* GetShaderResourceViewPtr(){return &TextureSRV;}
        ID3D11RenderTargetView* GetRenderTargetView(){return TextureRTV;}
        ID3D11RenderTargetView*const* GetRenderTargetViewPtr(){return &TextureRTV;}
        ID3D11DepthStencilView* GetDepthStencilView(){return TextureDSV;}
    private:

        /**
         * Cleans the created API-Resources
         */
        void CleanAPI();

        /**
         * Creates additional resources based on bind-flags
         */
        bool CreateAdditionalResources();

        // API-Specific texture interface
        ID3D11Texture2D* Texture2D;
        ID3D11RenderTargetView* TextureRTV;
        ID3D11ShaderResourceView* TextureSRV;
        ID3D11DepthStencilView* TextureDSV;
    };

#endif

}