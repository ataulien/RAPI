#pragma once

#ifdef RND_D3D11
#include "RD3D11BlendState.h"
#define RBLENDBASE_API RD3D11BlendState
#elif defined(RND_NULL)
#include "RNULLBlendState.h"
#define RBLENDBASE_API RNULLBlendState
#elif defined(RND_GL)
#include "RGLBlendState.h"
#define RBLENDBASE_API RGLBlendState
#endif

namespace RAPI
{
	class RBlendState : public RBLENDBASE_API
	{
	public:
		RBlendState(void);

		~RBlendState(void);

		/**
         * Creates the blendstate
         */
		bool CreateState(const RBlendStateInfo &info);
	};
}
