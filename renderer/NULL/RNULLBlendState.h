#pragma once

#include "RBaseBlendState.h"

namespace RAPI
{
    class RNULLBlendState : public RBaseBlendState
    {
    public:
        /**
         * API-Version of CreateState
         */
        bool CreateStateAPI(){return true;}

    };
}
