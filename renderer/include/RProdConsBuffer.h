#pragma once
#include <queue>
#include <mutex>
#include "RSemaphore.h"

namespace RAPI
{

/** Implementation of the producer/consumer pattern */
	template<typename T>
	class RProdConsBuffer
	{
	private:
		RProdConsBuffer(unsigned int ringBufferSize) : SemProd(ringBufferSize)
		{
			BufferDone = false;
			RingBuffer = new T[ringBufferSize];
			RingBufferSize = ringBufferSize;
			PositionConsumer = 0;
			PositionProducer = 0;
		}

		/** Lets the buffer wait until a new item has been produced. */
		T WaitItem()
		{
			SemCons.wait();
			unsigned int c = PositionConsumer;
			PositionConsumer = (PositionConsumer + 1) % RingBufferSize;

			// Notify the producer about one more space in the buffer
			SemProd.notify();

			return RingBuffer[c];
		}

		/** Puts a produced item into the buffer */
		void ProducedItem(T item)
		{
			SemProd.wait();

			RingBuffer[PositionProducer] = item;
			PositionProducer = (PositionProducer + 1) % RingBufferSize;

			// Notify the consumer about the new item
			SemCons.notify();
		}

		/** Returns true if this buffer is marked as done */
		bool IsDone()
		{ return BufferDone; }

	private:
		// Thread safety
		RSemaphore SemCons;
		RSemaphore SemProd;

		// Items produced
		std::queue<T> Produced;

		// True if there are no more items to be produced and none are to be consumed
		bool BufferDone;

		// For lock-free access to working-queue
		T *RingBuffer;
		unsigned int RingBufferSize;

		// Current position in the ringbuffer
		unsigned int PositionConsumer;
		unsigned int PositionProducer;
	};

}