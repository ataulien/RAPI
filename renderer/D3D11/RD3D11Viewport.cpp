#include "pch.h"
#ifdef RND_D3D11

#include "RD3D11Viewport.h"

using namespace RAPI;

RD3D11Viewport::RD3D11Viewport(void)
{
}


RD3D11Viewport::~RD3D11Viewport(void)
{
}

/**
 * Creates this viewport resources
 */
bool RD3D11Viewport::CreateViewportAPI()
{
	return true;
}
#endif