#pragma once

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
#include "RTimer.h"
#include "RBaseProfiler.h"

namespace RAPI
{

#ifdef RND_D3D11

    class RD3D11Profiler : public RBaseProfiler
    {

    public:

        struct ProfileData
        {
            ProfileData()
            {
                memset(DisjointQuery, 0, sizeof(DisjointQuery));
                memset(TimestampStartQuery, 0, sizeof(DisjointQuery));
                memset(TimestampEndQuery, 0, sizeof(DisjointQuery));
                LastGPUDelta = 0.0f;

                QueryStarted = FALSE;
                QueryFinished = FALSE;
            }

            ID3D11Query* DisjointQuery[QueryLatency];
            ID3D11Query* TimestampStartQuery[QueryLatency];
            ID3D11Query* TimestampEndQuery[QueryLatency];
            BOOL QueryStarted;
            BOOL QueryFinished;

            double LastGPUDelta;
        };


        void InitializeAPI(ID3D11Device* device, ID3D11DeviceContext* immContext);

        void StartProfileAPI(const std::string& name);
        void EndProfileAPI(const std::string& name);

        void EndFrameAPI(std::string& output);

    protected:

        typedef std::map<std::string, ProfileData> ProfileMap;

        ProfileMap profiles;
        UINT64 currFrame;

        ID3D11Device* device;
        ID3D11DeviceContext* context;

        RTimer timer;
    };
#endif
}