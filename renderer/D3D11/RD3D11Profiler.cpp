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

#ifdef RND_D3D11

#include "RD3D11Profiler.h"
#include "Logger.h"

using std::string;
using std::map;

// == Profiler ====================================================================================

using namespace RAPI;

void RD3D11Profiler::InitializeAPI(ID3D11Device* device, ID3D11DeviceContext* immContext)
{
	this->device = device;
	this->context = immContext;
}

void RD3D11Profiler::StartProfileAPI(const string& name)
{
	if(!device)
		return;

	HRESULT hr;

	ProfileData& profileData = profiles[name];
	_ASSERT(profileData.QueryStarted == FALSE);
	_ASSERT(profileData.QueryFinished == FALSE);

	if(profileData.DisjointQuery[currFrame] == NULL)
	{
		// Create the queries
		D3D11_QUERY_DESC desc;
		desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
		desc.MiscFlags = 0;
		LE(device->CreateQuery(&desc, &profileData.DisjointQuery[currFrame]));

		desc.Query = D3D11_QUERY_TIMESTAMP;
		LE(device->CreateQuery(&desc, &profileData.TimestampStartQuery[currFrame]));
		LE(device->CreateQuery(&desc, &profileData.TimestampEndQuery[currFrame]));

		profileData.LastGPUDelta = 0.0f;
	}

	// Start a disjoint query first
	context->Begin(profileData.DisjointQuery[currFrame]);

	// Insert the start timestamp    
	context->End(profileData.TimestampStartQuery[currFrame]);

	profileData.QueryStarted = TRUE;
}

void RD3D11Profiler::EndProfileAPI(const string& name)
{
	if(!device)
		return;

	ProfileData& profileData = profiles[name];
	_ASSERT(profileData.QueryStarted == TRUE);
	_ASSERT(profileData.QueryFinished == FALSE);

	// Insert the end timestamp    
	context->End(profileData.TimestampEndQuery[currFrame]);

	// End the disjoint query
	context->End(profileData.DisjointQuery[currFrame]);

	profileData.QueryStarted = FALSE;
	profileData.QueryFinished = TRUE;
}

void RD3D11Profiler::EndFrameAPI(std::string& output)
{
	if(!device)
		return;

	currFrame = (currFrame + 1) % QueryLatency;    

	float queryTime = 0.0f;

	// Iterate over all of the profiles
	ProfileMap::iterator iter;
	for(iter = profiles.begin(); iter != profiles.end(); iter++)
	{
		ProfileData& profile = (*iter).second;
		if(profile.QueryFinished == FALSE)
			continue;

		profile.QueryFinished = FALSE;

		if(profile.DisjointQuery[currFrame] == NULL)
			continue;

		// Get the query data
		UINT64 startTime = 0;
		while(context->GetData(profile.TimestampStartQuery[currFrame], &startTime, sizeof(startTime), 0) != S_OK);

		UINT64 endTime = 0;
		while(context->GetData(profile.TimestampEndQuery[currFrame], &endTime, sizeof(endTime), 0) != S_OK);

		D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
		while(context->GetData(profile.DisjointQuery[currFrame], &disjointData, sizeof(disjointData), 0) != S_OK);

		double time = 0.0f;
		if(disjointData.Disjoint == FALSE)
		{
			UINT64 delta = endTime - startTime;
			double frequency = static_cast<double>(disjointData.Frequency);
			time = (delta / frequency) * 1000.0f;

			profile.LastGPUDelta = time;
		}        

		output += (*iter).first + ": " + std::to_string(time) + "ms";
	}

	output += "Time spent waiting for queries: " + std::to_string(queryTime) + "ms";
}

#endif