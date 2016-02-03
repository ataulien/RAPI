#pragma once
#include "RBaseViewport.h"

namespace RAPI
{
	class RNULLViewport : public RBaseViewport
	{
	public:
		/**
         * Creates this viewport resources
         */
		bool CreateViewportAPI(){return true;}
	};
}