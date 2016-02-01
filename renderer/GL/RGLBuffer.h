#pragma once

#include "RBaseBuffer.h"

#ifdef RND_GL

namespace RAPI
{
    class RGLBuffer : public RBaseBuffer
    {
    public:
        /**
         * Creates the vertexbuffer with the given arguments
         */
        bool CreateBufferAPI(void *initData){return true;}

        /**
        * Maps the texture for update. Only possible with the right CPU-Acces and usage-flags.
        */
        bool MapAPI(void **dataOut){return true;}

        /**
        * Unmaps the texture
        */
        bool UnmapAPI(){return true;}

        /**
         * Updates the data of this buffer. If this isn't a dynamic resource, it will still try to update
         * the resource, but using a slower path
         */
        bool UpdateDataAPI(void *data, size_t dataSize = 0){return true;}

        /**
         * Deletes all resources this holds but keeps the object around.
         * Recreate the buffer by calling Init
         */
        void DeallocateAPI(){}
    };
}

#endif