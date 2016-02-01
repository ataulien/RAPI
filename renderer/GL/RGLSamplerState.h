#pragma once
#include "RBaseSamplerState.h"

#ifdef RND_GL
namespace RAPI
{
	class RGLSamplerState : public RBaseSamplerState
	{
	public:
		/**
		 * API-Version of CreateState
		 */
		bool CreateStateAPI(){return true;}
	};
}
#endif