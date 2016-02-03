#pragma once
#include "RBaseSamplerState.h"

namespace RAPI
{
	class RNULLSamplerState : public RBaseSamplerState
	{
	public:
		/**
		 * API-Version of CreateState
		 */
		bool CreateStateAPI(){return true;}
	};
}