#include "pch.h"

#ifdef RND_D3D11

#include "RD3D11DepthStencilState.h"
#include "REngine.h"
#include "RDevice.h"
#include "Logger.h"

using namespace RAPI;

RD3D11DepthStencilState::RD3D11DepthStencilState(void)
{
	State = nullptr;
}


RD3D11DepthStencilState::~RD3D11DepthStencilState(void)
{
	SafeRelease(State);
}

/**
 * API-Version of CreateState
 */
bool RD3D11DepthStencilState::CreateStateAPI()
{
	HRESULT hr;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

	// Depth test parameters
	depthStencilDesc.DepthEnable = StateInfo.DepthBufferEnabled;

	if(StateInfo.DepthWriteEnabled)
	{
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	}else
	{
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	}
	depthStencilDesc.DepthFunc = (D3D11_COMPARISON_FUNC)StateInfo.DepthBufferCompareFunc;

	// Stencil test parameters
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	LE(REngine::RenderingDevice->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &State));


	return true;
}

#endif