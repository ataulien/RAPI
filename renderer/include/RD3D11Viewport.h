#pragma once

#include "rbaseviewport.h"

namespace RAPI
{

#ifdef RND_D3D11

    class RD3D11Viewport :
        public RBaseViewport
    {
    public:
        RD3D11Viewport(void);
        ~RD3D11Viewport(void);

        /**
         * Creates this viewport resources
         */
        bool CreateViewportAPI();
    };

#endif
}