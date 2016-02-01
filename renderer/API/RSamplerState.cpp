#include "pch.h"
#include "RSamplerState.h"

using namespace RAPI;

RSamplerState::RSamplerState(void)
{
}


RSamplerState::~RSamplerState(void)
{
}

/**
 * Creates the blendstate
 */
bool RSamplerState::CreateState(const RSamplerStateInfo& info)
{
	StateInfo = info;
	return CreateStateAPI();
}