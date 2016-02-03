#pragma once
#include "RResource.h"

namespace RAPI
{

	struct RDepthStencilStateInfo
	{
		RDepthStencilStateInfo()
		{
			SetDefault();
		}

		/** Layed out for D3D11 */
		enum ECompareFunc
		{
			CF_COMPARISON_NEVER = 1,
			CF_COMPARISON_LESS = 2,
			CF_COMPARISON_EQUAL = 3,
			CF_COMPARISON_LESS_EQUAL = 4,
			CF_COMPARISON_GREATER = 5,
			CF_COMPARISON_NOT_EQUAL = 6,
			CF_COMPARISON_GREATER_EQUAL = 7,
			CF_COMPARISON_ALWAYS = 8
		};

		static const ECompareFunc DEFAULT_DEPTH_COMP_STATE = CF_COMPARISON_LESS_EQUAL;

		/** Sets the default values for this struct */
		RDepthStencilStateInfo &SetDefault()
		{
			DepthBufferEnabled = true;
			DepthWriteEnabled = true;
			DepthBufferCompareFunc = DEFAULT_DEPTH_COMP_STATE;

			return *this;
		}

		/** Depthbuffer settings */
		bool DepthBufferEnabled;
		bool DepthWriteEnabled;
		ECompareFunc DepthBufferCompareFunc;
	};

	class RBaseDepthStencilState : public RResource
	{
	public:
		RBaseDepthStencilState(void);

		virtual ~RBaseDepthStencilState(void);

		/**
        * Returns the stateinfo this state was made after
        */
		const RDepthStencilStateInfo &GetStateInfo() const
		{ return StateInfo; }

	protected:
		// General values about the state
		RDepthStencilStateInfo StateInfo;
	};

}