#include "pch.h"
#include "RGLBuffer.h"
#include "Logger.h"

using namespace RAPI;

RAPI::RGLBuffer::RGLBuffer()
{
	VertexBufferObject = 0;
}

RAPI::RGLBuffer::~RGLBuffer()
{
	DeallocateAPI();
}

/**
* Creates the vertexbuffer with the given arguments
*/
bool RGLBuffer::CreateBufferAPI(void *initData)
{
	glGenBuffers(1, &VertexBufferObject);
	CheckGlError();

	glBindBuffer (BindFlags, VertexBufferObject);
	CheckGlError();

	// Apply initial data and set size
	glBufferData (BindFlags, SizeInBytes, initData, Usage);
	CheckGlError();

	return true;
}

/**
* Maps the texture for update. Only possible with the right CPU-Acces and usage-flags.
*/
bool RGLBuffer::MapAPI(void **dataOut)
{
	// Get buffer pointer
	glBindBuffer (BindFlags, VertexBufferObject);
	void* ptr = glMapBuffer(BindFlags, GL_WRITE_ONLY);

	CheckGlError();

	if(!ptr)
		return false;

	*dataOut = ptr;

	return true;
}

/**
* Unmaps the texture
*/
bool RGLBuffer::UnmapAPI()
{
	// Unmap our buffer again
	glBindBuffer (BindFlags, VertexBufferObject);
	glUnmapBuffer(BindFlags);

	CheckGlError();

	return true;
}

/**
* Updates the data of this buffer. If this isn't a dynamic resource, it will still try to update
* the resource, but using a slower path
*/
bool RGLBuffer::UpdateDataAPI(void *data, size_t dataSize)
{
	if(dataSize != 0 && dataSize > GetSizeInBytes())
	{
		// Buffer too small for requested size, resize.
		DeallocateAPI();

		// Just set the new size and keep the old settings
		SizeInBytes = dataSize;

		// Create buffer and immediately set the data
		return CreateBufferAPI(data);
	}

	// Buffer is large enough, simply copy the data
	void* mappedData;
	if(!MapAPI(&mappedData))
		return false;

	memcpy(mappedData, data, dataSize != 0 ? dataSize : GetSizeInBytes());

	return UnmapAPI();
}

/**
* Deletes all resources this holds but keeps the object around.
* Recreate the buffer by calling Init
*/
void RGLBuffer::DeallocateAPI()
{
	// Clear our VBO
	glDeleteBuffers(1, &VertexBufferObject);
	CheckGlError();

	VertexBufferObject = 0;
}

/**
* Updates the VAO-Info of this buffer, if this is used as a vertexbuffer 
*/
void RGLBuffer::UpdateVAO(const RInputLayout* inputLayout)
{

}