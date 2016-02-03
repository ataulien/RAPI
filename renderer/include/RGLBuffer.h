#pragma once

#include "RBaseBuffer.h"

#ifdef RND_GL

namespace RAPI
{
	class RInputLayout;
    class RGLBuffer : public RBaseBuffer
    {
    public:

		RGLBuffer();
		~RGLBuffer();

        /**
         * Creates the vertexbuffer with the given arguments
         */
        bool CreateBufferAPI(void *initData);

        /**
        * Maps the texture for update. Only possible with the right CPU-Acces and usage-flags.
        */
        bool MapAPI(void **dataOut);

        /**
        * Unmaps the texture
        */
        bool UnmapAPI();

        /**
         * Updates the data of this buffer. If this isn't a dynamic resource, it will still try to update
         * the resource, but using a slower path
         */
        bool UpdateDataAPI(void *data, size_t dataSize = 0);

        /**
         * Deletes all resources this holds but keeps the object around.
         * Recreate the buffer by calling Init
         */
        void DeallocateAPI();

		/**
		 * Updates the VAO-Info of this buffer, if this is used as a vertexbuffer 
		 */
		void UpdateVAO(const RInputLayout* inputLayout);

		/**
		* Returns the vertex array object 
		*/
		GLuint GetVertexArrayObjectAPI(){return VertexArrayObject;}

		/**
		* Returns the vertex array object 
		*/
		GLuint GetVertexBufferObjectAPI(){return VertexBufferObject;}
	private:
		// The created VBO
		GLuint VertexBufferObject;

		// The used VAO
		GLuint VertexArrayObject;
    };
}

#endif