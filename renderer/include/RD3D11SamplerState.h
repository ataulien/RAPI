#pragma once
#include "rbasesamplerstate.h"

#ifdef RND_D3D11

namespace RAPI
{

	class RD3D11SamplerState :
			public RBaseSamplerState
	{
	public:
		RD3D11SamplerState(void);

		~RD3D11SamplerState(void);

		/**
         * API-Version of CreateState
         */
		bool CreateStateAPI();

		/** Getters */
		ID3D11SamplerState *GetState()
		{ return State; }

		ID3D11SamplerState *const *GetStatePtr()
		{ return &State; }

	private:
		// State object
		ID3D11SamplerState *State;
	};
}
#endif