#pragma once
#include "RTweakBarBase.h"

#ifdef RND_D3D11

namespace RAPI
{

	class RD3D11TweakBar : public RTweakBarBase
	{
	public:
		RD3D11TweakBar();

		~RD3D11TweakBar();

		/** Initializes the tweakbar */
		bool InitAPI();
	};
}

#endif