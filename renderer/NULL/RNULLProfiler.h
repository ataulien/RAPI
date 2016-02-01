#pragma once
#include "RBaseProfiler.h"

namespace RAPI
{
	class RNULLProfiler : public RBaseProfiler
	{
	public:
		struct ProfileData
		{
			ProfileData()
			{
				LastGPUDelta = 0.0f;

				QueryStarted = false;
				QueryFinished = false;
			}

			bool QueryStarted;
			bool QueryFinished;

			double LastGPUDelta;
		};

		void StartProfileAPI(const std::string& name){}
		void EndProfileAPI(const std::string& name){}

		void EndFrameAPI(std::string& output){}
	protected:
		typedef std::map<std::string, ProfileData> ProfileMap;

		ProfileMap profiles;
	};
}
