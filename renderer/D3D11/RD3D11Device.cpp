#include "pch.h"

#ifdef RND_D3D11

#include "RD3D11Device.h"
#include "Logger.h"
#include "RD3D11Texture.h"
#include "RStateMachine.h"
#include "RPipelineState.h"
#include "RTexture.h"
#include "RRasterizerState.h"
#include "RBlendState.h"
#include "RSamplerState.h"
#include "RDepthStencilState.h"
#include "RBuffer.h"
#include "RPixelShader.h"
#include "RVertexShader.h"
#include "RInputLayout.h"
#include "Types.h"
#include "RViewport.h"
#include "REngine.h"
#include "RThreadPool.h"
#include "RProfiler.h"

using namespace RAPI;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

//#define DEBUG_D3D11

const ETextureFormat DEFAULT_BACKBUFFER_FORMAT = ETextureFormat::TF_R8G8B8A8;

RD3D11Device::RD3D11Device()
{
	Device = nullptr;
	DXGIFactory = nullptr;
	DXGIAdapter = nullptr;
	ImmediateContext = nullptr;
	DXGISwapChain = nullptr;
	DepthStencilBuffer = nullptr;
	Backbuffer = nullptr;
}


RD3D11Device::~RD3D11Device()
{
	delete Backbuffer;
	delete DepthStencilBuffer;
	SafeRelease(Device);
	SafeRelease(DXGIFactory);
	SafeRelease(DXGIAdapter);
	SafeRelease(ImmediateContext);
	SafeRelease(DXGISwapChain);
}

/**
 * Creates the renderingdevice for the set API 
 */
bool RD3D11Device::CreateDeviceAPI()
{
	HRESULT hr;

	LogInfo() << "Initializing renderer...";

	// Find our graphics device
	if(!InitDXGI())
	{
		LogError() << "Failed to get GPU-Adapter!";
		return false;
	}

	// Set flags and featurelevel
	int flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	D3D_FEATURE_LEVEL featurelevel = D3D_FEATURE_LEVEL_11_0;

	// Create D3D11-Device
#ifndef DEBUG_D3D11
	LE(D3D11CreateDevice(DXGIAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, flags, &featurelevel, 1, D3D11_SDK_VERSION, &Device, NULL, &ImmediateContext));
#else
	LE(D3D11CreateDevice(DXGIAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, flags | D3D11_CREATE_DEVICE_DEBUG, &featurelevel, 1, D3D11_SDK_VERSION, &Device, NULL, &ImmediateContext));
#endif

	ThreadContexts[GetCurrentThreadId()].first = ImmediateContext;

	if(hr != S_OK)
		return false;

	ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Tell the profiler...
	Profiler.InitializeAPI(Device, ImmediateContext);

	return true;
}

/**
* Initializes DXGI-Resources like the adapter we are rendering on
*/
bool RD3D11Device::InitDXGI()
{
	HRESULT hr;

	// Create DXGI factory
	LE(CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&DXGIFactory));
	LE(DXGIFactory->EnumAdapters(0, &DXGIAdapter)); // Get first adapter

	if(!DXGIAdapter)
		return false;

	// Find out what we are rendering on to write it into the logfile
	DXGI_ADAPTER_DESC adpDesc;
	DXGIAdapter->GetDesc(&adpDesc);

	std::wstring wDeviceDescription(adpDesc.Description);
	std::string deviceDescription(wDeviceDescription.begin(), wDeviceDescription.end());

	LogInfo() << "Rendering on: " << deviceDescription.c_str();

	return true;
}


/**
* Sets the output window for this device
*/
bool RD3D11Device::SetWindowAPI()
{
	// Create swapchain
	CreateSwapChain(true);

	return true;
}

/**
* Called when the output-window has been resized
*/
bool RD3D11Device::OnResizeAPI()
{
	HRESULT hr;

	if(!DXGISwapChain) 
	{
		LogInfo() << "Can't resize non-existing swapchain!";
		return false;
	}

	// Get format from current backbuffer
	ETextureFormat format = Backbuffer != nullptr ? Backbuffer->GetTextureFormat() : DEFAULT_BACKBUFFER_FORMAT;

	// Make sure all resources are released
	ID3D11RenderTargetView* nullrtv[] = {nullptr};
	//ImmediateContext->OMSetRenderTargets(1, nullrtv,nullptr);

	// Unbind on all contexts
	for (auto c : ThreadContexts)
	{
		ID3D11DeviceContext* ctx = c.second.first;
		ctx->OMSetRenderTargets(1, nullrtv, nullptr);
	}

	delete Backbuffer;
	delete DepthStencilBuffer;

	// Resize the swapchain
	LE(DXGISwapChain->ResizeBuffers(1, OutputResolution.x, OutputResolution.y, (DXGI_FORMAT)format, 0));

	// Get backbuffer resource
	ID3D11Texture2D* backbuffer;
	DXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backbuffer);

	// Store it in our textureobject
	Backbuffer = new RTexture();
	Backbuffer->CreateDirect(backbuffer, OutputResolution, 1, format, (EBindFlags)(EBindFlags::B_RENDERTARGET | EBindFlags::B_SHADER_RESOURCE));

	DepthStencilBuffer = new RTexture();
	DepthStencilBuffer->CreateTexture(nullptr, 0, OutputResolution, 1, ETextureFormat::TF_FORMAT_R32_TYPELESS, EBindFlags::B_DEPTHSTENCIL);

	// Set as backbuffer
	ImmediateContext->OMSetRenderTargets(1, Backbuffer->GetRenderTargetViewPtr(), DepthStencilBuffer->GetDepthStencilView());

	// Set new viewport
	D3D11_VIEWPORT vp;
	vp.Height = (float)OutputResolution.y;
	vp.Width = (float)OutputResolution.x;
	vp.MaxDepth = 1.0f;
	vp.MinDepth = 0.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	ImmediateContext->RSSetViewports(1, &vp);

	return true;
}

/**
* Prepares a frame for rendering. Called between frame start and end
*/
bool RD3D11Device::PrepareFrameAPI()
{
	ImmediateContext->ClearRenderTargetView(Backbuffer->GetRenderTargetView(), (float *)&MainColorBufferClearColor);
	ImmediateContext->ClearDepthStencilView(DepthStencilBuffer->GetDepthStencilView(), D3D11_CLEAR_DEPTH, MainDepthBufferClearZ, 0);
	return true;
}

bool RD3D11Device::OnFrameStartAPI()
{
	// Initialize the immediate context
	PrepareContextAPI(GetCurrentThreadId());
	return true;
}

/**
* Prepares the renderingcontext on the given thread for drawing
*/
bool RD3D11Device::PrepareContextAPI(unsigned int threadId)
{
	ID3D11DeviceContext* ctx = GetThreadContext(threadId);
	ctx->OMSetRenderTargets(1, Backbuffer->GetRenderTargetViewPtr(), DepthStencilBuffer->GetDepthStencilView());
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set new viewport
	D3D11_VIEWPORT vp;
	vp.Height = (float)OutputResolution.y;
	vp.Width = (float)OutputResolution.x;
	vp.MaxDepth = 1.0f;
	vp.MinDepth = 0.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	ctx->RSSetViewports(1, &vp);

	return true;
}

bool RD3D11Device::OnFrameEndAPI()
{

	return true;
}

/**
* Presents the backbuffer on screen
*/
bool RD3D11Device::PresentAPI()
{
	//if(!ExecuteDeferredCommandLists())
	//	LogWarn() << "Failed to Execute Deferred Command Lists!";

	// Present the frame
	// TODO: VSync! DXGISwapChain->Present(1, 0)

	DXGISwapChain->Present(0, 0); 

	FrameCounter++;

	return true;
}



/**
* Registers a thread in the renderer. Creates a resources like a deferred context.
*/
bool RD3D11Device::RegisterThreadAPI(UINT threadID)
{
	HRESULT hr;

	// Check if this thread is already registered
	if(ThreadContexts.find(threadID) != ThreadContexts.end())
		return true;

	// Create a new context for the thread
	ID3D11DeviceContext* context;
	LE(Device->CreateDeferredContext(0, &context));

	// Put into our theadmap
	ThreadContexts[threadID].first = context;

	return hr == S_OK;
}

/**
* Creates the commandlist for the given threadID
*/
bool RD3D11Device::CreateCommandListForThreadAPI(UINT threadID)
{
	HRESULT hr;
	auto context = ThreadContexts[threadID];

	ID3D11CommandList* commandList;
	LE(context.first->FinishCommandList(false, &commandList));

	// Push the list into the touple of this context
	ThreadCommandListMutex.lock();
	context.second.push_back(commandList);
	ThreadCommandListMutex.unlock();

	return hr == S_OK;
}

ID3D11DeviceContext * RD3D11Device::GetThreadContext(UINT threadID)
{
	auto it = ThreadContexts.find(threadID);
	return it != ThreadContexts.end() ? (*it).second.first : nullptr;
}

ID3D11DeviceContext* RD3D11Device::GetImmediateContext()
{
	return ImmediateContext;
}

/**
* Creates a swapchain for the set output window
*/
bool RD3D11Device::CreateSwapChain(bool createWindowed)
{
	HRESULT hr;

	LogInfo() << "Creating new swapchain! (Format: DXGI_FORMAT_R8G8B8A8_UNORM)";

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	scd.OutputWindow = OutputWindow;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferDesc.Height = OutputResolution.y;
	scd.BufferDesc.Width = OutputResolution.x;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed = createWindowed;

	LE(DXGIFactory->CreateSwapChain(Device, &scd, &DXGISwapChain));

	if (!DXGISwapChain)
		return false;

	// Allocate resources
	OnResizeAPI();

	return true;
}

/**
* Executes all commandlists for all threads we have gotten so far
*/
bool RD3D11Device::ExecuteDeferredCommandLists()
{
	// Execute all commandlists we have
	ThreadCommandListMutex.lock();
	std::vector<ID3D11CommandList*> cmdLists;
	for each (auto keyvalue in ThreadContexts)
	{
		std::vector<ID3D11CommandList*>& lst = keyvalue.second.second;

		// Add all contexts to list
		cmdLists.insert(cmdLists.end(), lst.begin(), lst.end());

		// Prepare for next frame
		lst.clear();
	}
	ThreadCommandListMutex.unlock();

	// Execute them all
	for each (ID3D11CommandList* lst in cmdLists)
	{
		ImmediateContext->ExecuteCommandList(lst, false);
	}

	return true;
}

/**
 * Renders the given pipeline-state
 */
bool RD3D11Device::DrawPipelineStateAPI(const struct RPipelineState& state, const RStateMachine::ChangesStruct& changes, RStateMachine& stateMachine)
{
	// TODO: Slow? Profile!
	ID3D11DeviceContext* context = GetThreadContext(GetCurrentThreadId());

	// Bind everything
	if(DoDrawcalls)
	{
		BindPipelineState(state, changes, context, stateMachine);

	
		// Perform drawcall
		switch(state.IDs.DrawFunctionID)
		{
		case EDrawCallType::DCT_Draw:
			context->Draw(state.NumDrawElements, state.StartVertexOffset);
			break;

		case EDrawCallType::DCT_DrawIndexed:
			context->DrawIndexed(state.NumDrawElements, state.StartIndexOffset, state.StartVertexOffset);
			break;

		case EDrawCallType::DCT_DrawIndexedInstanced:
			context->DrawIndexedInstanced(state.NumDrawElements, state.NumInstances, state.StartIndexOffset, state.StartVertexOffset, state.StartInstanceOffset);
			break;
		}
	}

	return true;
}



/**
 * Renders an array of pipeline-states
 */
bool RD3D11Device::DrawPipelineStatesAPI(struct RPipelineState*const* stateArray, unsigned int numStates)
{
	// TODO: Rendering queue
	/*for(int i=0;i<numStates;i++)
		DrawPipelineStateAPI(*stateArray[i]);*/

	return true;
}

/**
 * Binds the resources of the given pipeline state
 */
bool RD3D11Device::BindPipelineState(const RPipelineState& state, const RStateMachine::ChangesStruct& changes, ID3D11DeviceContext* context, RStateMachine& stateMachine)
{
	stateMachine.SetFromPipelineState(&state, changes);
	const RPipelineStateFull& fs = stateMachine.GetCurrentState();

	if(changes.PrimitiveType)
		context->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)state.IDs.PrimitiveType);

	if(changes.RasterizerState && fs.RasterizerState)
		context->RSSetState(fs.RasterizerState->GetState());

	if(changes.BlendState && fs.BlendState)
		context->OMSetBlendState(fs.BlendState->GetState(), (float *)&RFloat4(0.0f,0.0f,0.0f,0.0f), 0xFFFFFFFF);

	if(changes.DepthStencilState && fs.DepthStencilState)
		context->OMSetDepthStencilState(fs.DepthStencilState->GetState(), 0);

	if(changes.SamplerState && fs.SamplerState)
		context->PSSetSamplers(0, 1, fs.SamplerState->GetStatePtr());

	for(int i=0;i<2;i++)
	{
		if(changes.VertexBuffers[i])
		{
			if(fs.VertexBuffers[i])
			{
				unsigned int offsets[] = { 0 };
				unsigned int strides[] = { fs.VertexBuffers[i]->GetStructuredByteSize() };
				context->IASetVertexBuffers(i, 1, fs.VertexBuffers[i]->GetBufferPtr(), strides, offsets);
			}
			else
			{
				unsigned int offsets[] = { 0 };
				unsigned int strides[] = { 0 };
				ID3D11Buffer* bf = nullptr;
				context->IASetVertexBuffers(i, 1, &bf, strides, offsets);
			}
		}
	}

	if(changes.IndexBuffer && fs.IndexBuffer)
		context->IASetIndexBuffer(fs.IndexBuffer->GetBuffer(), 
		fs.IndexBuffer->GetStructuredByteSize() == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);

	if(changes.PixelShader && fs.PixelShader)
		context->PSSetShader(fs.PixelShader->GetShader(), nullptr, 0);

	if(changes.VertexShader && fs.VertexShader)
		context->VSSetShader(fs.VertexShader->GetShader(), nullptr, 0);

	/*if(changes.GeometryShader)
	if(changes.HullShader)
	if(changes.DomainShader)*/ // TODO

	if(changes.InputLayout)
		context->IASetInputLayout(fs.InputLayout ? fs.InputLayout->GetInputLayout() : nullptr);

	// TODO: Do this for all shader stages
	// TODO: Structured buffers use the same registers as textures. A change of them will
	// not affect the changed state of the textures.
	if(changes.MainTexture)
	{
		for(unsigned int i=0;i<fs.Textures[EShaderType::ST_PIXEL].size();i++)
			if (!fs.Textures[EShaderType::ST_PIXEL].empty())
			{
				ID3D11ShaderResourceView* srv = fs.Textures[EShaderType::ST_PIXEL][i] ? 
					fs.Textures[EShaderType::ST_PIXEL][i]->GetShaderResourceView()
					: nullptr;
				context->PSSetShaderResources(i, 1, &srv);
			}
	}


	if(changes.ConstantBuffers[EShaderType::ST_VERTEX])
	{
		for(unsigned int j=0;j<fs.ConstantBuffers[EShaderType::ST_VERTEX].size();j++)
		{
			if(fs.ConstantBuffers[EShaderType::ST_VERTEX][j])
			{
				context->VSSetConstantBuffers(j, 1, fs.ConstantBuffers[EShaderType::ST_VERTEX][j]->GetBufferPtr());
			}
		}
	}

	if(changes.ConstantBuffers[EShaderType::ST_PIXEL])
	{
		for(unsigned int j=0;j<fs.ConstantBuffers[EShaderType::ST_PIXEL].size();j++)
		{
			if(fs.ConstantBuffers[EShaderType::ST_PIXEL][j])
			{
				context->PSSetConstantBuffers(j, 1, fs.ConstantBuffers[EShaderType::ST_PIXEL][j]->GetBufferPtr());
			}
		}
	}

	if(changes.StructuredBuffers[EShaderType::ST_VERTEX])
	{
		for(unsigned int j=0;j<fs.StructuredBuffers[EShaderType::ST_VERTEX].size();j++)
		{
			if(fs.StructuredBuffers[EShaderType::ST_VERTEX][j])
			{
				context->VSSetShaderResources(j, 1, fs.StructuredBuffers[EShaderType::ST_VERTEX][j]->GetBufferSRVPtr());
			}
		}
	}

	if(changes.Viewport && fs.Viewport)
	{
		context->RSSetViewports(1, (D3D11_VIEWPORT*)&fs.Viewport->GetViewportInfo());
	}

	return true;
}

/** Returns a list of available display modes */
bool RD3D11Device::GetDisplayModeListAPI(std::vector<DisplayModeInfo>& modeList, bool includeSuperSampling)
{
	HRESULT hr;
	UINT numModes = 0;
	DXGI_MODE_DESC* displayModes = NULL;
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	IDXGIOutput* output;

	// Get desktop rect
	RECT desktop;
	GetClientRect(GetDesktopWindow(), &desktop);

	if(!DXGIAdapter)
		return false;

	DXGIAdapter->EnumOutputs(0, &output);

	if(!output)
		return false;

	hr = output->GetDisplayModeList(format, 0, &numModes, NULL);

	displayModes = new DXGI_MODE_DESC[numModes];

	// Get the list
	hr = output->GetDisplayModeList(format, 0, &numModes, displayModes);

	

	for (unsigned int i = 0; i<numModes; i++)
	{
		if (displayModes[i].Format != format)
			continue;

		DisplayModeInfo info;
		info.Height = displayModes[i].Height;
		info.Width = displayModes[i].Width;
		info.Bpp = 32;

		if(info.Width > (unsigned long)desktop.right || info.Height > (unsigned long)desktop.bottom)
			continue; // Skip bigger sizes than the desktop rect, because DXGI doesn't like them apparently TODO: Fix this if possible!

		if(!modeList.empty() && memcmp(&modeList.back(), &info, sizeof(info)) == 0)
			continue; // Already have that in list

		modeList.push_back(info);
	}

	if(includeSuperSampling)
	{
		// Put supersampling resolutions in, up to just below 8k
		int i = 2;
		DisplayModeInfo ssBase = modeList.back();
		while(ssBase.Width * i < 8192 && ssBase.Height * i < 8192)
		{
			DisplayModeInfo info;
			info.Height = ssBase.Height * i;
			info.Width = ssBase.Width * i;
			info.Bpp = 32;

			modeList.push_back(info);
			i++;
		}
	}


	delete[] displayModes;

	output->Release();

	return true;
}

/**
* Returns the resolution needed for the given window
*/
RInt2 RD3D11Device::GetWindowResolutionAPI(WindowHandle hWnd)
{
    RECT r;
    GetClientRect(OutputWindow, &r);

    return RInt2(r.right - r.left, r.bottom - r.top);
}

#endif