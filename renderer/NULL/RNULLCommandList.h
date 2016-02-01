#pragma once
#include "RBaseCommandList.h"

namespace RAPI
{
	class RNULLCommandList : public RBaseCommandList
	{
	public:
		/** Initializes a commandlist for the given Thread ID */
		bool InitAPI(){return true;}

		/** Creates the commandlist and makes it ready to be played back */
		bool FinalizeCommandListAPI(){return true;}

		/** Plays the generated commandlist back on the main-thread */
		bool ExecuteCommandListAPI(){return true;}
	};
}