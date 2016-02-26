#pragma once
#include "RResource.h"

namespace RAPI
{
	struct RRasterizerStateInfo
	{
		RRasterizerStateInfo()
		{
			SetDefault();
		}

		/** Layed out for D3D11 */
		enum ECullMode
		{
			CM_CULL_NONE = 1,
			CM_CULL_FRONT = 2,
			CM_CULL_BACK = 3
		};


		/** Sets the default values for this struct */
		RRasterizerStateInfo &SetDefault()
		{
			CullMode = CM_CULL_BACK;
			ZBias = 0;
			FrontCounterClockwise = true;
			Wireframe = false;
			DepthClipEnable = false;

			return *this;
		}

		ECullMode CullMode;
		bool FrontCounterClockwise;
		bool DepthClipEnable;
		int ZBias;
		bool Wireframe;
	};

	class RBaseRasterizerState :
			public RResource
	{
	public:
		RBaseRasterizerState(void);

		virtual ~RBaseRasterizerState(void);

		/**
        * Returns the stateinfo this state was made after
        */
		const RRasterizerStateInfo &GetStateInfo() const
		{ return StateInfo; }

	protected:
		// General values about the state
		RRasterizerStateInfo StateInfo;
	};
}
