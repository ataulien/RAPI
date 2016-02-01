#pragma once
#include "RBaseBlendState.h"


#ifdef RND_D3D11

namespace RAPI
{
	class RD3D11BlendState :
		public RBaseBlendState
	{
	public:
		RD3D11BlendState(void);
		~RD3D11BlendState(void);

		/**
		 * API-Version of CreateState
		 */
		bool CreateStateAPI();

		/** Getters */
		ID3D11BlendState* GetState(){ return State; }

	private:
		// State object
		ID3D11BlendState* State;
	};
}
#endif