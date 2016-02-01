#include "pch.h"

#ifdef RND_D3D11

#include "RD3D11BlendState.h"
#include "REngine.h"
#include "RDevice.h"
#include "Logger.h"

using namespace RAPI;

RD3D11BlendState::RD3D11BlendState(void)
{
	State = nullptr;
}


RD3D11BlendState::~RD3D11BlendState(void)
{
	SafeRelease(State);
}

/**
 * API-Version of CreateState
 */
bool RD3D11BlendState::CreateStateAPI()
{
	HRESULT hr;
	D3D11_BLEND_DESC blendDesc;

	// Set to default
	blendDesc.AlphaToCoverageEnable = StateInfo.AlphaToCoverage;
	blendDesc.IndependentBlendEnable = FALSE;

	blendDesc.RenderTarget[0].RenderTargetWriteMask = StateInfo.ColorWritesEnabled ? (D3D11_COLOR_WRITE_ENABLE_RED | 
		D3D11_COLOR_WRITE_ENABLE_BLUE | 
		D3D11_COLOR_WRITE_ENABLE_GREEN |
		D3D11_COLOR_WRITE_ENABLE_ALPHA) : 0;

	blendDesc.RenderTarget[0].SrcBlend = (D3D11_BLEND)StateInfo.SrcBlend;
	blendDesc.RenderTarget[0].DestBlend = (D3D11_BLEND)StateInfo.DestBlend;
	blendDesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)StateInfo.BlendOp;
	blendDesc.RenderTarget[0].SrcBlendAlpha = (D3D11_BLEND)StateInfo.SrcBlendAlpha;
	blendDesc.RenderTarget[0].DestBlendAlpha = (D3D11_BLEND)StateInfo.DestBlendAlpha;
	blendDesc.RenderTarget[0].BlendOpAlpha = (D3D11_BLEND_OP)StateInfo.BlendOpAlpha;
	blendDesc.RenderTarget[0].BlendEnable = StateInfo.BlendEnabled;

	LE(REngine::RenderingDevice->GetDevice()->CreateBlendState(&blendDesc, &State));


	return true;
}
#endif