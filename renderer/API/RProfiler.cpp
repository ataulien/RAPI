//=================================================================================================
//
//	Query Profiling Sample
//  by MJP
//  http://mynameismjp.wordpress.com/
//
//  All code and content licensed under Microsoft Public License (Ms-PL)
//
//=================================================================================================

#include "pch.h"
#include "RProfiler.h"
#include "Logger.h"

using namespace RAPI;

// Switch profiling on or off
//#define ENABLE_PROFILING

using std::string;
using std::map;

// == Profiler ====================================================================================

RProfiler RProfiler::GlobalProfiler;

void RProfiler::StartProfile(const string& name)
{
#ifndef ENABLE_PROFILING
	return;
#endif

	StartProfileAPI(name);

}

void RProfiler::EndProfile(const string& name)
{
#ifndef ENABLE_PROFILING
	return;
#endif

	EndProfileAPI(name);
}

void RProfiler::EndFrame(std::string& output)
{
	EndFrameAPI(output);
}

RProfiler::RProfileResult RProfiler::GetLastProfileResult(const std::string& name)
{
	RProfileResult r;
	r.GPUTime = profiles[name].LastGPUDelta;
	return r;
}

std::vector<std::pair<std::string, RProfiler::RProfileResult>> RProfiler::GetAllProfileResults()
{
	std::vector<std::pair<std::string, RProfiler::RProfileResult>> out;
	for(auto p : profiles)
	{
		if(p.second.LastGPUDelta > 0)
		{
			out.push_back(std::make_pair(p.first, GetLastProfileResult(p.first)));
		}
	}

	return out;
}

// == ProfileBlock ================================================================================

RProfileBlock::RProfileBlock(const std::string& name) : name(name)
{
    RProfiler::GlobalProfiler.StartProfile(name);
}

RProfileBlock::~RProfileBlock()
{
    RProfiler::GlobalProfiler.EndProfile(name);
}