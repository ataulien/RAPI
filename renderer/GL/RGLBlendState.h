#pragma once

#include "RBaseBlendState.h"

#ifdef RND_GL
namespace RAPI
{
    class RGLBlendState : public RBaseBlendState
    {
    public:
        /**
         * API-Version of CreateState
         */
        bool CreateStateAPI(){return true;}

    };
}
#endif