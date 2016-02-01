#include "pch.h"
#ifdef RND_D3D11

#include "RD3D11SamplerState.h"
#include "REngine.h"
#include "RDevice.h"
#include "Logger.h"

using namespace RAPI;

RD3D11SamplerState::RD3D11SamplerState(void)
{
	State = nullptr;
}


RD3D11SamplerState::~RD3D11SamplerState(void)
{
	SafeRelease(State);
}

/**
 * API-Version of CreateState
 */
bool RD3D11SamplerState::CreateStateAPI()
{
	HRESULT hr;

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = (D3D11_FILTER)StateInfo.Filter;
	samplerDesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)StateInfo.AddressU;
	samplerDesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)StateInfo.AddressV;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = StateInfo.MaxAnisotropy;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MinLOD = -3.402823466e+38F; // -FLT_MAX
	samplerDesc.MaxLOD = 3.402823466e+38F; // FLT_MAX

	LE(REngine::RenderingDevice->GetDevice()->CreateSamplerState(&samplerDesc, &State));


	return true;
}
#endif