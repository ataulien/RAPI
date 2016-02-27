#pragma once
#include "RBaseBuffer.h"

#ifdef RND_D3D11
#include "RD3D11Buffer.h"
#define RBUFFERBASE_API RD3D11Buffer
#elif defined(RND_NULL)
#include "RNULLBuffer.h"
#define RBUFFERBASE_API RNULLBuffer
#elif defined(RND_GL)
#include "RGLBuffer.h"
#define RBUFFERBASE_API RGLBuffer
#endif

namespace RAPI
{

	class RBuffer :
			public RBUFFERBASE_API
	{
	public:
		RBuffer();

		~RBuffer();

		/**
		 * Creates the vertexbuffer with the given arguments
		 */
		bool Init(const void *initData,
				  unsigned int sizeInBytes,
				  unsigned int structuredByteSize,
				  EBindFlags bindFlags = B_VERTEXBUFFER,
				  EUsageFlags usage = EUsageFlags::U_DEFAULT,
				  ECPUAccessFlags cpuAccess = ECPUAccessFlags::CA_NONE,
				  const std::string &fileName = "");

		/**
		* Maps the buffer for update. Only possible with the right CPU-Acces and usage-flags.
		*/
		bool Map(void **dataOut);

		/**
		* Unmaps the buffer
		*/
		bool Unmap();

		/**
		 * Updates the data of this buffer. If this isn't a dynamic resource, it will still try to update
		 * the resource, but using a slower path. Also, if the newly specified size if greater than 0
		 * and also greater than the current size, the buffer will be recreated using the specified size
		 */
		bool UpdateData(const void *data, size_t dataSize = 0);


		/**
		 * Deletes all resources this holds but keeps the object around.
		 * Recreate the buffer by calling Init
		 */
		void Deallocate();
	};
}