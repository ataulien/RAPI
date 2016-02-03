#pragma once
#include "pch.h"
#include "RBuffer.h"
#include "RResourceCache.h"
#include "REngine.h"

namespace RAPI
{

	template<typename T>
	class RBufferCollection : public RResource
	{
	public:
		RBufferCollection(void)
		{
			Buffer = REngine::ResourceCache->CreateResource<RBuffer>();
			BufferInitialized = false;
		}

		~RBufferCollection(void)
		{
			REngine::ResourceCache->DeleteResource(Buffer);
		}

		/** Adds more data to the buffer. Returns offset in units. */
		unsigned int AddData(T *data, unsigned int num)
		{
			for (unsigned int i = 0; i < num; i++)
				TempDataStorage.push_back(data[i]);

			return TempDataStorage.size() - num;
		}

		/** Adds more data to the buffer. Returns offset in units. */
		template<typename S>
		unsigned int AddDataCast(S *data, unsigned int num)
		{
			for (unsigned int i = 0; i < num; i++)
				TempDataStorage.push_back((T) data[i]);

			return TempDataStorage.size() - num;
		}

		/** Has to be called after all the data has been placed into the collection */
		bool Construct(EBindFlags bindFlags = EBindFlags::B_VERTEXBUFFER, EUsageFlags usage = EUsageFlags::U_IMMUTABLE,
					   bool deleteData = true)
		{
			if (TempDataStorage.empty())
				return false;

			if (!BufferInitialized || usage != EUsageFlags::U_DYNAMIC) {
				// Remove old buffer if we already have one
				Buffer->Deallocate();

				// Copy to GPU
				if (!Buffer->Init(&TempDataStorage[0],
								  TempDataStorage.size() * sizeof(T),
								  sizeof(T),
								  bindFlags,
								  usage,
								  usage == EUsageFlags::U_DYNAMIC ? ECPUAccessFlags::CA_WRITE
																  : ECPUAccessFlags::CA_NONE))
					return false;

				BufferInitialized = true;
			}
			else {
				// Just update data. Resizes if needed.
				Buffer->UpdateData(&TempDataStorage[0], TempDataStorage.size() * sizeof(T));
			}

			// Only delete data if the user doesn't need it anymore
			if (deleteData) {
				TempDataStorage.clear();
				TempDataStorage.shrink_to_fit(); // Clear memory as well
			}

			return true;
		}

		/** Returns the buffer the data is in */
		RBuffer *GetBuffer()
		{
			return Buffer;
		}

	private:
		/** Buffer big enough to hold the data given to this */
		RBuffer *Buffer;
		bool BufferInitialized;

		/** Temporary vector for collecting ingoing data */
		std::vector<T> TempDataStorage;
	};

}