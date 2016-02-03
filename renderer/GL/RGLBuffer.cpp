#include "pch.h"
#include "RGLBuffer.h"
#include "Logger.h"
#include "RInputLayout.h"

using namespace RAPI;

RAPI::RGLBuffer::RGLBuffer()
{
	VertexBufferObject = 0;
	VertexArrayObject = 0;
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
	// Get input element desc
	const INPUT_ELEMENT_DESC* desc = inputLayout->GetInputElementDesc();

	// Already initialized?
	if(VertexArrayObject || BindFlags != GL_ARRAY_BUFFER)
		return;

	// Create VAO
	glGenVertexArrays(1, &VertexArrayObject);
	CheckGlError();

	glBindVertexArray(VertexArrayObject);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);

	for(int i = 0; i < inputLayout->GetNumInputDescElements(); i++)
	{
		const INPUT_ELEMENT_DESC& d = desc[i];
		
		glEnableVertexAttribArray(d.InputSlot);
		CheckGlError();

		// Unpack the formats
		switch(d.Format)
		{
		case FORMAT_R32G32B32A32_FLOAT:
			glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
			break;

		case FORMAT_R32G32B32_FLOAT:
			glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
			break;

		case FORMAT_R32G32_FLOAT:
			glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
			break;

		case FORMAT_R8G8B8A8_UNORM:
			glVertexAttribPointer(i, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, nullptr);
			break;

		default:
			LogWarnBox() << "Unknown INPUT_ELEMENT_DESC-Format: " << d.Format;
			return;
		}

		CheckGlError();
		
	}
}