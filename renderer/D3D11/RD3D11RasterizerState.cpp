#include "pch.h"
#ifdef RND_D3D11

#include "RD3D11RasterizerState.h"
#include "REngine.h"
#include "RDevice.h"
#include "Logger.h"

using namespace RAPI;

RD3D11RasterizerState::RD3D11RasterizerState(void)
{
	State = nullptr;
}


RD3D11RasterizerState::~RD3D11RasterizerState(void)
{
	SafeRelease(State);
}

/**
 * API-Version of CreateState
 */
bool RD3D11RasterizerState::CreateStateAPI()
{
	HRESULT hr;

	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.CullMode = (D3D11_CULL_MODE)StateInfo.CullMode;

	if(StateInfo.Wireframe)
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	else
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;

	rasterizerDesc.FrontCounterClockwise = StateInfo.FrontCounterClockwise;
	rasterizerDesc.DepthBias = StateInfo.ZBias;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = StateInfo.DepthClipEnable;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	LE(REngine::RenderingDevice->GetDevice()->CreateRasterizerState(&rasterizerDesc, &State));

	return true;
}
#endif