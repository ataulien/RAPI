#pragma once
#include "RBaseBuffer.h"
#ifdef RND_D3D11

namespace RAPI
{
	// Numbers of frames should have buffers to prepare for
	const int NUM_BUFFERSTASH_FRAME_STORAGES = 3;

	class RD3D11Buffer :
		public RBaseBuffer
	{
	public:
		RD3D11Buffer();
		~RD3D11Buffer();

		/**
		* Creates the vertexbuffer with the given arguments
		*/
		bool CreateBufferAPI(void* initData);

		/**
		* Maps the texture for update. Only possible with the right CPU-Acces and usage-flags.
		*/
		bool MapAPI(void** dataOut);

		/**
		* Unmaps the texture
		*/
		bool UnmapAPI();

		/**
		 * Updates the data of this buffer. If this isn't a dynamic resource, it will still try to update
		 * the resource, but using a slower path
		 */
		bool UpdateDataAPI(void* data, size_t dataSize = 0);

		/**
		 * Getters, doublebuffered for dynamic buffers!
		 */
		ID3D11Buffer* GetBuffer()
		{
			return Buffer;
		}

		ID3D11Buffer*const* GetBufferPtr()
		{
			return &Buffer;
		}

		ID3D11ShaderResourceView* GetBufferSRV()
		{
			return BufferSRV;
		}

		ID3D11ShaderResourceView*const* GetBufferSRVPtr()
		{
			return &BufferSRV;
		}

		/**
		 * Deletes all resources this holds but keeps the object around.
		 * Recreate the buffer by calling Init
		 */
		void DeallocateAPI();
	private:

		/** Switches to the next buffer in the stash, if we're doing maps on the same frame
			Returns true if switched. */
		bool TrySwitchBuffers();

		// Buffer to hold data on the GPU
		ID3D11Buffer* Buffer;

		// Shader resource view for structured data
		ID3D11ShaderResourceView* BufferSRV;

		// Multiple allocated buffers for a dynamic type to reduce
		// GPU-Sync points. Only active with the usage is set to DYNAMIC
		std::pair<ID3D11Buffer*, ID3D11ShaderResourceView*> BufferStash[NUM_BUFFERSTASH_FRAME_STORAGES];

		// Last frame this buffer was updated on
		unsigned int LastFrameUpdated;

		// Current slot of the BufferStash we are using
		unsigned int StashBufferRotation;

	};

}
#endif