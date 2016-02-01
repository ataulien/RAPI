#include "pch.h"
#include "RBuffer.h"

using namespace RAPI;


RBuffer::RBuffer()
{
}


RBuffer::~RBuffer()
{
}

/** 
* Creates the vertexbuffer with the given arguments
*/
bool RBuffer::Init(void *initData,
                   unsigned int sizeInBytes,
                   unsigned int structuredByteSize,
                   EBindFlags bindFlags,
                   EUsageFlags usage,
                   ECPUAccessFlags cpuAccess,
                   const std::string &fileName)
{
    SizeInBytes = sizeInBytes;
    BindFlags = bindFlags;
    Usage = usage;
    CpuAccess = cpuAccess;
    FileName = fileName;
    StructuredByteSize = structuredByteSize;

    return CreateBufferAPI(initData);
}

/**
* Maps the texture for update. Only possible with the right CPU-Acces and usage-flags.
*/
bool RBuffer::Map(void **dataOut)
{
    if (!MapAPI(dataOut))
        return false;

    return true;
}

/**
* Unmaps the texture
*/
bool RBuffer::Unmap()
{
    if (!UnmapAPI())
        return false;

    return true;
}

/**
 * Updates the data of this buffer. If this isn't a dynamic resource, it will still try to update
 * the resource, but using a slower path
 */
bool RBuffer::UpdateData(void *data, size_t dataSize)
{
    return UpdateDataAPI(data, dataSize);
}

/**
* Deletes all resources this holds but keeps the object around.
* Recreate the buffer by calling Init
*/
void RBuffer::Deallocate()
{
    DeallocateAPI();
}
