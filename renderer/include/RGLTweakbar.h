#pragma once
#include "RTweakBarBase.h"

#ifdef RND_GL
namespace RAPI
{
	class RGLTweakbar : public RTweakBarBase
	{
	public:
		/** Initializes the tweakbar */
		bool InitAPI(){return true;}
	};

}
#endif
