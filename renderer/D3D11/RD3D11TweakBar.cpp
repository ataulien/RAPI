#include "pch.h"

#ifdef RND_D3D11

#include "RD3D11TweakBar.h"
#include "Logger.h"
#include "REngine.h"
#include "RDevice.h"

using namespace RAPI;

RD3D11TweakBar::RD3D11TweakBar()
{
}


RD3D11TweakBar::~RD3D11TweakBar()
{
}

/** Initializes the tweakbar */
bool RD3D11TweakBar::InitAPI()
{
#ifdef USE_TWEAK_BAR
	LogInfo() << "Initializing AntTweakBar";
	if(!TwInit(TW_DIRECT3D11, REngine::RenderingDevice->GetDevice()))
		return XR_FAILED;


	TwWindowSize(REngine::RenderingDevice->GetOutputResolution().x, REngine::RenderingDevice->GetOutputResolution().y);
#endif
	return true;
}
#endif