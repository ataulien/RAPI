#pragma once
#include "pch.h"
#include "RResourceCache.h"
#include "RStateMachine.h"
#include "RProfiler.h"

namespace RAPI {
/**
 * Simple renderqueue to hold states for a stage 
 */
	struct RRenderQueue {
		// Vector holding the pipeline states to draw
		std::vector<const RPipelineState *> Queue;

		// This will have the same size as the Queue after processing this renderqueue is done
		// and contain the changes from the i-1'th pipeline-state to the i'th.
		std::vector<RStateMachine::ChangesStruct> Changes;

#ifndef PUBLIC_RELASE
		// TODO: Debug, take out!
		std::vector<class GBaseDrawable *> Sources;
#endif

		// If true, this queue can be sorted and the rendering order doesn't matter
		bool SortQueue;

		// Flag if this queue is free
		bool InUse;

		// Same size as threads in the thread pool. Will contain the finished commandlists
		// for this queue after processing.
		std::vector<class RCommandList *> QueueCommandLists;
		std::vector<std::future<void>> QueueCommandListFutures;

		// Generation of changes and commandlists can be multithreaded. Use this to determine if the process was completed.
		std::future<void> ProcessedFuture;

		// Name of this queue
		std::string Name;
	};

	typedef unsigned int RRenderQueueID;

	class RBaseDevice {
	public:
		RBaseDevice();

		virtual ~RBaseDevice();

		/**
         * Returns the resolution of the window we are currently drawing to
         */
		const int2 &GetOutputResolution() { return OutputResolution; }

		/**
         * Access to the underlaying state machine
         */
		RStateMachine &GetStateMachine() { return StateMachine; }

		/**
         * Sets the clear values for the main buffers
         */
		void SetMainClearValues(const float4 &color, float z = 1.0f) {
			MainColorBufferClearColor = color;
			MainDepthBufferClearZ = z;
		}

	protected:
		// Output-Window for the swapchain
		WindowHandle OutputWindow;

		// Output resolution of backbuffer
		int2 OutputResolution;

		// State machine for the device. Helps to reduce unneeded statechanges.
		RStateMachine StateMachine;

		// Counter of how many frames since the start of the program have been rendered
		unsigned int FrameCounter;

		// Counter of drawcalls queued
		unsigned int QueuedDrawCallCounter;

		// Counter of active queues
		unsigned int QueueCounter;

		// Queued drawcalls
		std::vector<RRenderQueue *> RenderQueue;

		// Values to clear the main buffers with when a new frame is started
		float4 MainColorBufferClearColor;
		float MainDepthBufferClearZ;

		// Main profiler
		RProfiler Profiler;

		// Whether to do drawcalls
		bool DoDrawcalls;
	};

}