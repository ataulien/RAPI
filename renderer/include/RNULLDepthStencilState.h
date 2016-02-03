#pragma once
#include "RBaseDepthStencilState.h"

namespace RAPI
{
	class RNULLDepthStencilState : public RBaseDepthStencilState
	{
	public:
		/**
         * API-Version of CreateState
         */
		bool CreateStateAPI(){return true;}
	};

}