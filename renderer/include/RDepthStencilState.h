#pragma once

#ifdef RND_D3D11
#include "RD3D11DepthStencilState.h"
#define RDEPTHSTENCILBASE_API RD3D11DepthStencilState
#elif defined(RND_NULL)
#include "RNULLDepthStencilState.h"
#define RDEPTHSTENCILBASE_API RNULLDepthStencilState
#elif defined(RND_GL)
#include "RGLDepthStencilState.h"
#define RDEPTHSTENCILBASE_API RGLDepthStencilState
#endif

namespace RAPI
{
	class RDepthStencilState :
			public RDEPTHSTENCILBASE_API
	{
	public:
		RDepthStencilState(void);

		~RDepthStencilState(void);

		/**
		 * Initializes the state
		 */
		bool CreateState(const RDepthStencilStateInfo &info);

	private:

	};
}