#pragma once
#include "RBaseRasterizerState.h"

#ifdef RND_GL
namespace RAPI
{
	class RGLRasterizerState : public RBaseRasterizerState
	{
	public:
		/**
		 * API-Version of CreateState
		 */
		bool CreateStateAPI(){return true;}
	};

}
#endif