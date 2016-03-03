#pragma once

#include "RBaseBuffer.h"

#ifdef RND_GL

namespace RAPI
{
	// Numbers of frames should have buffers to prepare for
	const int NUM_BUFFERSTASH_FRAME_STORAGES = 3;

	class RBuffer;
	class RInputLayout;
    class RGLBuffer : public RBaseBuffer
    {
    public:

		RGLBuffer();
		~RGLBuffer();

        /**
         * Creates the vertexbuffer with the given arguments
         */
        bool CreateBufferAPI(const void *initData);

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
        bool UpdateDataAPI(const void *data, size_t dataSize = 0);

        /**
         * Deletes all resources this holds but keeps the object around.
         * Recreate the buffer by calling Init
         */
        void DeallocateAPI();

		/**
		 * Updates the VAO-Info of this buffer, if this is used as a vertexbuffer 
		 */
		void UpdateVAO(const RInputLayout* inputLayout, RBuffer* instanceBuffer);

		/**
		* Returns the vertex array object 
		*/
		GLuint GetVertexArrayObjectAPI(){return VertexArrayObject;}

		/**
		* Returns the vertex array object 
		*/
		GLuint GetBufferObjectAPI(){return VertexBufferObject;}
	private:

		/** Switches to the next buffer in the stash, if we're doing maps on the same frame 
			Returns true if switched. */
		bool TrySwitchBuffers();

		// The created VBO
		GLuint VertexBufferObject;

		// The used VAO
		GLuint VertexArrayObject;

		// Multiple allocated buffers for a dynamic type to reduce
		// GPU-Sync points. Only active with the usage is set to DYNAMIC
		std::pair<GLuint, GLuint> BufferStash[NUM_BUFFERSTASH_FRAME_STORAGES];

		// Last frame this buffer was updated on
		unsigned int LastFrameUpdated;

		// Current slot of the BufferStash we are using
		unsigned int StashBufferRotation;
    };
}

#endif