#pragma once
#include "RBaseRasterizerState.h"

namespace RAPI
{
	class RNULLRasterizerState : public RBaseRasterizerState
	{
	public:
		/**
		 * API-Version of CreateState
		 */
		bool CreateStateAPI(){return true;}
	};

}