#pragma once

#ifdef RND_D3D11
#include "RD3D11SamplerState.h"
#define RSAMPLERBASE_API RD3D11SamplerState
#elif defined(RND_NULL)
#include "RNULLSamplerState.h"
#define RSAMPLERBASE_API RNULLSamplerState
#elif defined(RND_GL)
#include "RGLSamplerState.h"
#define RSAMPLERBASE_API RGLSamplerState
#endif

namespace RAPI
{

	class RSamplerState : public RSAMPLERBASE_API
	{
	public:
		RSamplerState(void);

		~RSamplerState(void);

		/**
		 * Creates the blendstate
		 */
		bool CreateState(const RSamplerStateInfo &info);
	};

}