#include "pch.h"

#ifdef RND_D3D11

#include "RD3D11CommandList.h"
#include "REngine.h"
#include "RDevice.h"
#include "Logger.h"

using namespace RAPI;

RD3D11CommandList::RD3D11CommandList()
{
	CommandList = nullptr;
}


RD3D11CommandList::~RD3D11CommandList()
{
	SafeRelease(CommandList);
}

/** Initializes a commandlist for the given Thread ID */
bool RD3D11CommandList::InitAPI()
{
	// Make sure we have a context
	REngine::RenderingDevice->RegisterThread(GetCurrentThreadId());

	return true;
}

/** Creates the commandlist and makes it ready to be played back */
bool RD3D11CommandList::FinalizeCommandListAPI()
{
	// Make sure we have a context
	REngine::RenderingDevice->RegisterThread(GetCurrentThreadId());

	// Get the context of this thread
	ID3D11DeviceContext* ctx = REngine::RenderingDevice->GetThreadContext(GetCurrentThreadId());

	if (!ctx)
	{
		LogError() << "Context for thread " << GetCurrentThreadId() << " not initialized!";
		return false;
	}

	// Make sure we are clean
	SafeRelease(CommandList);

	// Create the commandlist
	HRESULT hr;
	LE(ctx->FinishCommandList(false, &CommandList));

	return hr == S_OK;
}

/** Plays the generated commandlist back on the main-thread */
bool RD3D11CommandList::ExecuteCommandListAPI()
{
	if (!CommandList)
		return false;

	// Execute on immediate context
	// TODO: Last parameter should be false, but some of the last textures in the frame get lost then!
	REngine::RenderingDevice->GetImmediateContext()->ExecuteCommandList(CommandList, true);
	SafeRelease(CommandList);

	return true;
}
#endif