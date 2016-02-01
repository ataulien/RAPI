#pragma once
#include "RBaseViewport.h"

#ifdef RND_GL
namespace RAPI
{
	class RGLViewport : public RBaseViewport
	{
	public:
		/**
         * Creates this viewport resources
         */
		bool CreateViewportAPI(){return true;}
	};
}
#endif