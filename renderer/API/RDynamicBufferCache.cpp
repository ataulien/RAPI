#include "pch.h"
#include "RDynamicBufferCache.h"
#include "REngine.h"
#include "RResourceCache.h"
#include "RBuffer.h"

using namespace RAPI;


RDynamicBufferCache::RDynamicBufferCache(void)
{
    Frame = 0;
}


RDynamicBufferCache::~RDynamicBufferCache(void)
{
    // TODO: Free memory!!
}

/** Request a dynamic buffer from the stash */
RCachedDynamicBuffer RDynamicBufferCache::GetDataBuffer(EBindFlags bindFlags, unsigned int size,
                                                        unsigned int stride)
{
    // Check if such a buffer is free
    // Map of the current free buffers of the given bindflags
    auto &bfreeMap = FrameBuffers[Frame].FreeBuffers[bindFlags];
    auto it = bfreeMap.find(size);
    if (it == bfreeMap.end())
    {
        auto &bAllMap = FrameBuffers[Frame].AllBuffers[bindFlags];
        // Make a new buffer, but don't put it into the free-list just yet
        return RCachedDynamicBuffer(Frame, MakeBufferFor(bAllMap, bindFlags, size, stride));
    }

    // Use any found free buffer
    RBuffer *buffer = (*it).second;
    buffer->SetStructuredByteSize(stride);

    // Remove it from the free-map
    bfreeMap.erase(it);

    return RCachedDynamicBuffer(Frame, buffer);
}

/** Signals the cache that we're done with a buffer */
void RDynamicBufferCache::DoneWith(RBuffer *buffer, unsigned int bufferFrame, EBindFlags bindFlags)
{
    auto &bDoneMap = FrameBuffers[bufferFrame].DoneBuffers[bindFlags];

    // TODO: Update size in main buffer-map!

    // Insert it into the done-map in its frame
    bDoneMap.insert(std::unordered_multimap<unsigned int, RBuffer *>::value_type(buffer->GetSizeInBytes(), buffer));
}

void RDynamicBufferCache::DoneWith(RCachedDynamicBuffer &buffer)
{
    DoneWith(buffer.Buffer, buffer.Frame, buffer.Buffer->GetBindFlags());
}

/** Called by the Device when the frame ended */
void RDynamicBufferCache::OnFrameEnded()
{
    // Move to next frame
    Frame = (Frame + 1) % NUM_BUFFERCACHE_FRAME_STORAGES;

    // Move the done buffers from this frame to the free ones
    MarkFrameAsFree(Frame);
}

/** Inserts a new buffer into the current frames map */
RBuffer *RDynamicBufferCache::MakeBufferFor(std::unordered_multimap<unsigned int, RBuffer *> &map,
                                            EBindFlags bindFlags, unsigned int size, unsigned int stride)
{
    // Create wanted buffer
    RBuffer *buffer = REngine::ResourceCache->CreateResource<RBuffer>();
    buffer->Init(nullptr, size, stride, bindFlags, EUsageFlags::U_DYNAMIC, ECPUAccessFlags::CA_WRITE,
                 "Dynamic cached buffer");

    // Insert into map
    map.insert(std::unordered_multimap<unsigned int, RBuffer *>::value_type(size, buffer));

    return buffer;
}

/** Marks all buffers in the given frame as free */
void RDynamicBufferCache::MarkFrameAsFree(unsigned int frame)
{
    // Merge the done-map to the free-map
    for (auto &dones : FrameBuffers[frame].DoneBuffers)
    {
        for (auto &var : dones.second)
        {
            unsigned int size = var.first;
            RBuffer *buffer = var.second;
            FrameBuffers[frame].FreeBuffers[dones.first].insert(
                    std::unordered_multimap<unsigned int, RBuffer *>::value_type(size, buffer));
        }

    }

    // Clear the done-map
    FrameBuffers[frame].DoneBuffers.clear();
}

/** Clears unused buffers from the cache */
void RDynamicBufferCache::ClearCache()
{
    for (int i = 0; i < NUM_BUFFERCACHE_FRAME_STORAGES; i++)
    {
        for (auto &dones : FrameBuffers[i].DoneBuffers)
        {
            for (auto &var : dones.second)
            {
                REngine::ResourceCache->DeleteResource(var.second);
            }
        }
        FrameBuffers[i].DoneBuffers.clear();

        for (auto &dones : FrameBuffers[i].FreeBuffers)
        {
            for (auto &var : dones.second)
            {
                REngine::ResourceCache->DeleteResource(var.second);
            }
        }
        FrameBuffers[i].FreeBuffers.clear();
    }
}