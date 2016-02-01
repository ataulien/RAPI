#pragma once
#include "rbasedepthstencilstate.h"

#ifdef RND_D3D11

namespace RAPI
{

	class RD3D11DepthStencilState :
			public RBaseDepthStencilState
	{
	public:
		RD3D11DepthStencilState(void);

		~RD3D11DepthStencilState(void);


		/**
         * API-Version of CreateState
         */
		bool CreateStateAPI();

		/** Getters */
		ID3D11DepthStencilState *GetState()
		{ return State; }

	private:
		// State object
		ID3D11DepthStencilState *State;
	};
}

#endif