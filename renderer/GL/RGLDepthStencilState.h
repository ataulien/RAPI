#pragma once
#include "RBaseDepthStencilState.h"

#ifdef RND_GL
namespace RAPI
{
	class RGLDepthStencilState : public RBaseDepthStencilState
	{
	public:
		/**
         * API-Version of CreateState
         */
		bool CreateStateAPI(){return true;}
	};

}
#endif