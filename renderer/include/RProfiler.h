//=================================================================================================
//
//	Query Profiling Sample
//  by MJP
//  http://mynameismjp.wordpress.com/
//
//  All code and content licensed under Microsoft Public License (Ms-PL)
//
//=================================================================================================

#pragma once

#include "pch.h"
#include "RTimer.h"

#ifdef RND_D3D11
#include "RD3D11Profiler.h"
#define RPROFILERBASE_API RD3D11Profiler
#elif defined(RND_NULL)
#include "RNULLProfiler.h"
#define RPROFILERBASE_API RNULLProfiler
#elif defined(RND_GL)
#include "RGLProfiler.h"
#define RPROFILERBASE_API RGLProfiler
#endif

namespace RAPI
{

	class RProfiler : public RPROFILERBASE_API
	{

	public:

		struct RProfileResult
		{
			double GPUTime;
		};

		static RProfiler GlobalProfiler;

		/** Starts/Ends a profile-block */
		void StartProfile(const std::string &name);

		void EndProfile(const std::string &name);

		/** Has to be called once on frame-end */
		void EndFrame(std::string &output);

		/** Returns a single profile-result. If not yet present, the GPUTime will be 0. */
		RProfileResult GetLastProfileResult(const std::string &name);

		/** Returns a list of all finished profile results */
		std::vector<std::pair<std::string, RProfileResult>> GetAllProfileResults();

	protected:
	};

	class RProfileBlock
	{
	public:

		RProfileBlock(const std::string &name);

		~RProfileBlock();


	protected:

		std::string name;
	};

}