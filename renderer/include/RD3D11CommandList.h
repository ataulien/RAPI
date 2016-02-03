#pragma once
#include "RBaseCommandList.h"

namespace RAPI
{
#ifdef RND_D3D11

    class RD3D11CommandList :
        public RBaseCommandList
    {
    public:
        RD3D11CommandList();
        ~RD3D11CommandList();

        /** Initializes a commandlist for the given Thread ID */
        bool InitAPI();

        /** Creates the commandlist and makes it ready to be played back */
        bool FinalizeCommandListAPI();

        /** Plays the generated commandlist back on the main-thread */
        bool ExecuteCommandListAPI();
    protected:
        // Commandlist interface
        ID3D11CommandList* CommandList;
    };

#endif
}