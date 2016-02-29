#pragma once

#include <string.h>
#include "RResource.h"
#include "RResourceCache.h"
#include "REngine.h"
#include "RBuffer.h"
#include "RObservable.h"

namespace RAPI
{

	template<typename T>
	struct RLogicalBuffer
	{
		~RLogicalBuffer()
		{
			delete[] DataArray;
		}

		T *DataArray;
		unsigned int PageStart; // In elements
		unsigned int PageNumElements;
	};

	template<typename T>
	class RPagedBuffer : public RResource, public RObservable
	{
	public:
		RPagedBuffer(RAPI::EBindFlags bindFlags)
		{
			IsDirty = false;
			DataBuffer = REngine::ResourceCache->CreateResource<RBuffer>();

			BindFlags = bindFlags;
		}

		~RPagedBuffer()
		{
			REngine::ResourceCache->DeleteResource(DataBuffer);

			for (RLogicalBuffer<T> *b : LogicalBuffers) {
				delete b;
			}
		}

		/** Adds a databuffer to this paged buffer and returns a pointer to
			the created logical buffer. */
		RLogicalBuffer<T> *AddLogicalBuffer(const T *data, unsigned int numElements)
		{
			RLogicalBuffer<T> *lb = new RLogicalBuffer<T>();

			// Copy data
			lb->DataArray = new T[numElements];
			memcpy(lb->DataArray, data, sizeof(T) * numElements);
			lb->PageNumElements = numElements;

			lb->PageStart = 0; // This will be initialized on rebuild-pages.

			LogicalBuffers.insert(lb);
			IsDirty = true;

			return lb;
		}

		/** Removes a logical databuffer from the internal set */
		void RemoveBuffer(RLogicalBuffer<T> *buffer)
		{
			LogicalBuffers.erase(buffer);
			IsDirty = true;
		}

		/** Rebuilds the internal buffer. This operation can take a lot of time! */
		void RebuildPages()
		{
			// Don't do more work than needed
			if (!IsDirty)
				return;

			// First, make a buffer containing all of the pages data
			std::vector<T> fullData;

			for (RLogicalBuffer<T> *b : LogicalBuffers) {
				// Set page start
				b->PageStart = fullData.size();
				for (unsigned int i = 0; i < b->PageNumElements; i++) {
					fullData.push_back(b->DataArray[i]);
				}
			}

			// Notify everything that we constructed the final buffer
			// and give a pointer to it so they can modify the data
			NotifyAll(0, fullData.data());

			// Now update our buffers contents
			DataBuffer->Deallocate();
			DataBuffer->Init(fullData.data(),
							 fullData.size() * sizeof(T),
							 sizeof(T),
							 BindFlags,
							 EUsageFlags::U_IMMUTABLE,
							 ECPUAccessFlags::CA_NONE,
							 "Paged buffer");

			IsDirty = false;
		}

		/** Returns a pointer to the internal databuffer */
		RBuffer *GetBuffer()
		{
			return DataBuffer;
		}

	private:
		// Map of logical buffers, by ID
		std::set<RLogicalBuffer<T> *> LogicalBuffers;

		// If true, a "RebuildPages" would change the internal buffers contents
		bool IsDirty;

		// The real buffer on the GPU containing our data after a call to "RebuildPages"
		RBuffer *DataBuffer;

		// Buffer information
		EBindFlags BindFlags;
	};

}