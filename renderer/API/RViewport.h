#pragma once
#include "rbaseviewport.h"

#ifdef RND_D3D11
#include "RD3D11Viewport.h"
#define RVIEWPORTBASE_API RD3D11Viewport
#elif defined(RND_NULL)
#include "RNULLViewport.h"
#define RVIEWPORTBASE_API RNULLViewport
#elif defined(RND_GL)
#include "RGLViewport.h"
#define RVIEWPORTBASE_API RGLViewport
#endif

namespace RAPI
{

	class RViewport :
			public RVIEWPORTBASE_API
	{
	public:
		RViewport(void);

		~RViewport(void);

		/**
		 * Creates this viewport resources
		 */
		bool CreateViewport(const ViewportInfo &viewport);
	};

}