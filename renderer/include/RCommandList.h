#pragma once
#include "pch.h"

#ifdef RND_D3D11
#include "RD3D11CommandList.h"
#define RCOMMANDLIST_API RD3D11CommandList
#elif defined(RND_NULL)
#include "RNULLCommandList.h"
#define RCOMMANDLIST_API RNULLCommandList
#elif defined(RND_GL)
#include "RGLCommandList.h"
#define RCOMMANDLIST_API RGLCommandList
#endif

namespace RAPI
{


	class RCommandList :
			public RCOMMANDLIST_API
	{
	public:
		RCommandList();

		~RCommandList();

		/** Initializes a commandlist for the given Thread ID */
		bool Init();

		/** Creates the commandlist and makes it ready to be played back.
			This must be called from an other thread than the main-thread! */
		bool FinalizeCommandList();

		/** Plays the generated commandlist back on the main-thread */
		bool ExecuteCommandList();
	};

}