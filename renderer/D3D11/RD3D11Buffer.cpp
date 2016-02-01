#include "pch.h"

#ifdef RND_D3D11

#include "RD3D11Buffer.h"
#include "REngine.h"
#include "Logger.h"
#include "RD3D11SyncCheck.h"

using namespace RAPI;

const bool USE_DOUBLEBUFFERING = true;

RD3D11Buffer::RD3D11Buffer()
{
	Buffer = nullptr;
	BufferSRV = nullptr;

	LastFrameUpdated = 0;
	StashBufferRotation = 0;

	memset(BufferStash, 0, sizeof(BufferStash));
}


RD3D11Buffer::~RD3D11Buffer()
{
	DeallocateAPI();
}

/** 
* Creates the vertexbuffer with the given arguments
*/
bool RD3D11Buffer::CreateBufferAPI(void * initData)
{
	unsigned int buffersToCreate = 1;

	// In case we dont have data, allocate some to satisfy D3D11
	char* data = NULL;
	if (!initData)
	{
		data = new char[SizeInBytes];
		memset(data, 0, SizeInBytes);

		initData = data;
	}

	HRESULT hr;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = SizeInBytes;
	bufferDesc.Usage = (D3D11_USAGE)Usage;
	bufferDesc.BindFlags = (D3D11_BIND_FLAG)BindFlags;
	bufferDesc.CPUAccessFlags = (D3D11_CPU_ACCESS_FLAG)CpuAccess;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = StructuredByteSize;

	// Check for structured buffer
	if((BindFlags & EBindFlags::B_SHADER_RESOURCE) != 0)
		bufferDesc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA srData;
	srData.pSysMem = initData;
	srData.SysMemPitch = 0;
	srData.SysMemSlicePitch = 0;

	// Make more buffers if this is supposed to be dynamic
	if((Usage & EUsageFlags::U_DYNAMIC) != 0 && USE_DOUBLEBUFFERING)
		buffersToCreate = NUM_BUFFERSTASH_FRAME_STORAGES;

	for(unsigned int i=0;i<buffersToCreate;i++)
	{
		LE(REngine::RenderingDevice->GetDevice()->CreateBuffer(&bufferDesc, &srData, &BufferStash[i].first));

		// Check for structured buffer again to create the SRV
		if((BindFlags & EBindFlags::B_SHADER_RESOURCE) != 0)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory( &srvDesc, sizeof(srvDesc) );
			srvDesc.Format              = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension       = D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.ElementWidth = SizeInBytes / StructuredByteSize;

			REngine::RenderingDevice->GetDevice()->CreateShaderResourceView(BufferStash[i].first, &srvDesc, &BufferStash[i].second);
		}	
	}

	// Delete array of allocated data to statisfy d3d11, if needed
	delete[] data;

	// There is at least one filled element in the stah
	Buffer = BufferStash[0].first;
	BufferSRV = BufferStash[0].second;

	return true;
}

/**
* Maps the texture for update. Only possible with the right CPU-Acces and usage-flags.
*/
bool RD3D11Buffer::MapAPI(void ** dataOut)
{
	// Switch buffers if needed to avoid cpu stall
	TrySwitchBuffers();

	HRESULT hr;
	ID3D11DeviceContext* context = REngine::RenderingDevice->GetThreadContext(GetCurrentThreadId());

	D3D11_MAPPED_SUBRESOURCE res;
	RD3D11_CTX_SYNC_CHECK_R(LE(context->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res)));

	*dataOut = res.pData;

	return hr == S_OK;
}

/**
* Unmaps the texture
*/
bool RD3D11Buffer::UnmapAPI()
{
	ID3D11DeviceContext* context = REngine::RenderingDevice->GetThreadContext(GetCurrentThreadId());
	context->Unmap(Buffer, 0);

	return true;
}

/**
 * Updates the data of this buffer. If this isn't a dynamic resource, it will still try to update
 * the resource, but using a slower path
 */
bool RD3D11Buffer::UpdateDataAPI(void* data, size_t dataSize)
{
	if(dataSize != 0 && dataSize > GetSizeInBytes())
	{
		// Buffer to small for requested size, resize.
		DeallocateAPI();

		// Just set the new size and keep the old settings
		SizeInBytes = dataSize;

		// Create buffer and immediately set the data
		return CreateBufferAPI(data);
	}

	// Buffer is large enough, simply copy the data
	void* mappedData;
	if(!MapAPI(&mappedData))
		return false;

	memcpy(mappedData, data, dataSize != 0 ? dataSize : GetSizeInBytes());

	return UnmapAPI();
}

/** Switches to the next buffer in the stash, if we're doing maps on the same frame 
		Returns true if switched. */
bool RD3D11Buffer::TrySwitchBuffers()
{
	// Only on dynamic buffers
	if((Usage & EUsageFlags::U_DYNAMIC) == 0 || !USE_DOUBLEBUFFERING)
		return false;

	unsigned int frame = REngine::RenderingDevice->GetFrameCounter();

	// Check if we were updated on the current or last frame
	if(LastFrameUpdated == frame || LastFrameUpdated + 1 == frame)
	{
		// We were, do the switch!
		StashBufferRotation = (StashBufferRotation + 1) % NUM_BUFFERSTASH_FRAME_STORAGES;
		Buffer = BufferStash[StashBufferRotation].first;
		BufferSRV = BufferStash[StashBufferRotation].second;
		return true;
	}

	LastFrameUpdated = frame;

	return false;
}

/**
* Deletes all resources this holds but keeps the object around.
* Recreate the buffer by calling Init
*/
void RD3D11Buffer::DeallocateAPI()
{
	// We have everything in the stash
	for(unsigned int i=0;i<NUM_BUFFERSTASH_FRAME_STORAGES;i++)
	{
		SafeRelease(BufferStash[i].first);
		SafeRelease(BufferStash[i].second);
	}

	Buffer = nullptr;
	BufferSRV = nullptr;

	LastFrameUpdated = 0;
	StashBufferRotation = 0;

	memset(BufferStash, 0, sizeof(BufferStash));
}
#endif