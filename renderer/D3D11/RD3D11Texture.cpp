#include "pch.h"
#ifdef RND_D3D11

#include "RD3D11Texture.h"
#include "REngine.h"
#include "Logger.h"

#pragma comment(lib, "d3dx11.lib")

using namespace RAPI;

RD3D11Texture::RD3D11Texture()
{
	Texture2D = nullptr;
	TextureRTV = nullptr;
	TextureSRV = nullptr;
	TextureDSV = nullptr;
}


RD3D11Texture::~RD3D11Texture()
{
	CleanAPI();
}

/**
* Loads a texture from the given file
*/
bool RD3D11Texture::CreateFromFileAPI(const std::string& file)
{
	HRESULT hr;

	D3DX11CreateShaderResourceViewFromFile(REngine::RenderingDevice->GetDevice(),
		file.c_str(),
		nullptr,
		nullptr,
		&TextureSRV, &hr);

	return hr == S_OK;
}

/**
* Deletes all resources this holds but keeps the object around.
* Recreate the texture by calling CreateTexture
*/
void RD3D11Texture::DeallocateAPI()
{
	CleanAPI();
}

/**
* Creates the textureobjects for the given API 
*/
bool RD3D11Texture::CreateTextureAPI(const void* textureData,
	std::vector<void*> mipData)
{
	HRESULT hr;

	// TODO: Allow for more formats
	if((BindFlags & D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL) != 0)
	{
		if(TextureFormat != ETextureFormat::TF_FORMAT_R32_TYPELESS)
		{
			LogError() << "RD3D11Texture with D3D11_BIND_DEPTH_STENCIL-Flag needs to have TF_FORMAT_R32_TYPELESS set!";
			return false;
		}

		if(ArraySize != 1)
		{
			LogError() << "RD3D11Texture with D3D11_BIND_DEPTH_STENCIL-Flag can't have arraySize greater than 1!";
			return false;
		}
	}

	size_t size = SizeInBytes;

	// Clean up first, so we can keep these texture-objects
	CleanAPI();

	// Restore size, as CleanAPI set it to 0
	SizeInBytes = size;

	CD3D11_TEXTURE2D_DESC textureDesc(
		(DXGI_FORMAT)TextureFormat,
		Resolution.x,
		Resolution.y,
		ArraySize,
		NumMipLevels,
		(D3D11_BIND_FLAG)BindFlags, (D3D11_USAGE)UsageFlags, 0, 1, 0, 0);

	D3D11_SUBRESOURCE_DATA sub;
	ZeroMemory(&sub, sizeof(sub));
	sub.pSysMem = textureData;
	sub.SysMemSlicePitch = RBaseTexture::ComputeSizeInBytes(0, Resolution, TextureFormat);
	sub.SysMemPitch = ComputeRowPitchBytes(0);

	std::vector<D3D11_SUBRESOURCE_DATA> subvector;
	subvector.resize(NumMipLevels);

	subvector[0] = sub;

	// Assign some memory for the mipmaps.
	// All mips use the original data for now, until they got updated,
	// because the pSysMem-Pointer needs to be valid
	for (unsigned int i = 0; i < mipData.size(); i++)
	{
		ZeroMemory(&subvector[i+1], sizeof(D3D11_SUBRESOURCE_DATA));
		subvector[i+1].pSysMem = mipData[i];
		subvector[i+1].SysMemSlicePitch = RBaseTexture::ComputeSizeInBytes(i+1, Resolution, TextureFormat);
		subvector[i+1].SysMemPitch = ComputeRowPitchBytes(i+1);
	}

	// Create main resources
	LE(REngine::RenderingDevice->GetDevice()->CreateTexture2D(&textureDesc, 
		textureData != nullptr ? &subvector[0] : nullptr, 
		&Texture2D));

	// Create SRV, RTV if wanted
	CreateAdditionalResources();

	IsFullyInitialized = true;

	return true;
}
/**
* Creates this texture directly from an API-Resource
*/
bool RD3D11Texture::CreateDirect(ID3D11Texture2D * texture,
								 RInt2 resolution,
								 unsigned int numMipLevels,
								 ETextureFormat textureFormat,
								 EBindFlags bindFlags,
								 EUsageFlags usageFlags)
{
	// Clean up first, so we can keep these texture-objects
	CleanAPI();

	TextureFormat = textureFormat;
	SizeInBytes = 0;
	Resolution = resolution;
	NumMipLevels = numMipLevels;
	BindFlags = bindFlags;
	UsageFlags = usageFlags;
	Texture2D = texture;

	// Create SRV, RTV if wanted
	CreateAdditionalResources();

	IsFullyInitialized = true;

	return true;
}

/**
* Updates a subresource with default usage
*/
bool RD3D11Texture::UpdateSubresourceAPI(void* data, int mipLevel, int arrayIndex)
{
	ID3D11DeviceContext* ctx = REngine::RenderingDevice->GetThreadContext(GetCurrentThreadId());

	ctx->UpdateSubresource(Texture2D, 
		D3D11CalcSubresource(mipLevel, arrayIndex, NumMipLevels), 
		nullptr,
		data,
		0,
		0);

	return true;
}

/**
* Maps the texture for update. Only possible with the right CPU-Acces and usage-flags.
*/
bool RD3D11Texture::MapAPI(void ** dataOut, int subresource)
{
	HRESULT hr;
	ID3D11DeviceContext* context = REngine::RenderingDevice->GetThreadContext(GetCurrentThreadId());

	D3D11_MAPPED_SUBRESOURCE res;
	LE(context->Map(Texture2D, subresource, D3D11_MAP_WRITE_DISCARD, 0, &res));

	*dataOut = res.pData;

	return hr == S_OK;
}

/**
* Unmaps the texture
*/
bool RD3D11Texture::UnmapAPI(int subresource)
{
	ID3D11DeviceContext* context = REngine::RenderingDevice->GetThreadContext(GetCurrentThreadId());
	context->Unmap(Texture2D, subresource);

	return true;
}

/**
* Cleans the created API-Resources
*/
void RD3D11Texture::CleanAPI()
{
	IsFullyInitialized = false;

	SafeRelease(Texture2D);
	SafeRelease(TextureRTV);
	SafeRelease(TextureSRV);
	SafeRelease(TextureDSV);

	// Reset this so we know this texture isn't valid anymore
	SizeInBytes = 0;
}

/**
* Creates additional resources based on bind-flags
*/
bool RD3D11Texture::CreateAdditionalResources()
{
	HRESULT hr;

	// Create other resources based on bind-flags

	if((BindFlags & D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET) != 0)
		LE(REngine::RenderingDevice->GetDevice()->CreateRenderTargetView(Texture2D, nullptr, &TextureRTV));

	// Check if this is supposed to be a depth-stencil 
	if((BindFlags & D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL) != 0)
	{
		//Create a render target view
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = CD3D11_DEPTH_STENCIL_VIEW_DESC();
		ZeroMemory( &descDSV, sizeof( descDSV ) );
		descDSV.Format = DXGI_FORMAT_D32_FLOAT; // ZODO: More formats
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		descDSV.Flags=0;

		LE(REngine::RenderingDevice->GetDevice()->CreateDepthStencilView(Texture2D, &descDSV, &TextureDSV));

		// Make sure to use the right format for the SRV
		if((BindFlags & D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE) != 0)
		{
			CD3D11_SHADER_RESOURCE_VIEW_DESC descSRV(D3D11_SRV_DIMENSION::D3D_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32_FLOAT);
			LE(REngine::RenderingDevice->GetDevice()->CreateShaderResourceView(Texture2D, &descSRV, &TextureSRV));
		}
	}else
	{
		// Create the SRV as usual
		if((BindFlags & D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE) != 0)
			LE(REngine::RenderingDevice->GetDevice()->CreateShaderResourceView(Texture2D, nullptr, &TextureSRV));
	}

	return true;
}

/** 
* Tries to save this texture to a file. Only works for RGBA8-Format.
*/
bool RD3D11Texture::SaveToFileAPI(const std::string& file)
{
	HRESULT hr;
	LE_RB(D3DX11SaveTextureToFile(REngine::RenderingDevice->GetImmediateContext(), Texture2D, D3DX11_IFF_PNG, file.c_str()));

	return true;
}
#endif