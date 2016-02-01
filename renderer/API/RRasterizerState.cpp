#include "pch.h"
#include "RRasterizerState.h"

namespace RAPI
{

	RRasterizerState::RRasterizerState(void)
	{
	}


	RRasterizerState::~RRasterizerState(void)
	{
	}

/**
 * Creates the blendstate
 */
	bool RRasterizerState::CreateState(const RRasterizerStateInfo &info)
	{
		StateInfo = info;
		return CreateStateAPI();
	}
}