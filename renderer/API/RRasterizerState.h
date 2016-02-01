#pragma once

#ifdef RND_D3D11
#include "RD3D11RasterizerState.h"
#define RRASTERIZERBASE_API RD3D11RasterizerState
#elif defined(RND_NULL)
#include "RNULLRasterizerState.h"
#define RRASTERIZERBASE_API RNULLRasterizerState
#elif defined(RND_GL)
#include "RGLRasterizerState.h"
#define RRASTERIZERBASE_API RGLRasterizerState
#endif

namespace RAPI
{

	class RRasterizerState : public RRASTERIZERBASE_API
	{
	public:
		RRasterizerState(void);

		~RRasterizerState(void);

		/**
		 * Creates the blendstate
		 */
		bool CreateState(const RRasterizerStateInfo &info);
	};

}