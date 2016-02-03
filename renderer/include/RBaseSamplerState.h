#pragma once
#include "RResource.h"

namespace RAPI
{
/** Sampler state information */
	struct RSamplerStateInfo
	{
		RSamplerStateInfo()
		{
			SetDefault();
		}

		/** Sets the default values for this struct */
		RSamplerStateInfo &SetDefault()
		{
			AddressU = TA_WRAP;
			AddressV = TA_WRAP;
			Filter = EFilter::FILTER_ANISOTROPIC;
			MaxAnisotropy = 16;

			return *this;
		}

		unsigned int MaxAnisotropy;
		EFilter Filter;
		ETextureAddress AddressU;
		ETextureAddress AddressV;
	};

	class RBaseSamplerState :
			public RResource
	{
	public:
		RBaseSamplerState(void);

		~RBaseSamplerState(void);

		/**
        * Returns the stateinfo this state was made after
        */
		const RSamplerStateInfo &GetStateInfo() const
		{ return StateInfo; }

	protected:
		// General values about the state
		RSamplerStateInfo StateInfo;
	};
}
