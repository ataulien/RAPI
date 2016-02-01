#include "pch.h"
#include "RDevice.h"
#include "Logger.h"
#include <algorithm>
#include <assert.h>
#include "REngine.h"
#include "RThreadPool.h"
#include "RCommandList.h"
#include "RBuffer.h"
#include "RTools.h"

namespace RAPI
{


#if !defined(NO_MULTITHREADED_RENDERING) && defined(R_PROFILE_QUEUES)
#define NO_MULTITHREADED_RENDERING
#endif

	const unsigned int MIN_STATES_FOR_THREADED_RENDER = 2;

	RDevice::RDevice()
	{
	}


	RDevice::~RDevice()
	{
		RTools::DeleteElements(RenderQueue);
	}

/**
* Creates the renderingdevice for the set API 
*/
	bool RDevice::CreateDevice()
	{
		if (!CreateDeviceAPI())
			return false;



		// Make deferred contexts for all threads
		std::vector<size_t> threadIDs = REngine::ThreadPool->getThreadIDs();
		for (unsigned int i = 0; i < threadIDs.size(); i++) {
			RegisterThread(threadIDs[i]);
		}

		return true;
	}

/**
* Sets the output window for this device
*/
	bool RDevice::SetWindow(WindowHandle hWnd)
	{
		OutputWindow = hWnd;

		if (!SetWindowAPI())
			return false;

		// Init tweak-bar for debugging
		RTools::TweakBar.Init();
		AddTweakBarValues();

		return true;
	}

/**
* Registers a thread in the renderer. Creates a resources like a deferred context.
*/
	bool RDevice::RegisterThread(uint32_t threadID)
	{
		return RegisterThreadAPI(threadID);
	}

/**
* Creates the commandlist for the given threadID
*/
	bool RDevice::CreateCommandListForThread(uint32_t threadID)
	{
		return CreateCommandListForThreadAPI(threadID);
	}

/**
* Called on frame-start 
*/
	bool RDevice::OnFrameStart()
	{
		// Prepare for new frame
		StateMachine.Invalidate();

		Profiler.StartProfile("Frame");

		// Grab all finished commandlists
		return OnFrameStartAPI();
	}


/**
* Called on frame-end
*/
	bool RDevice::OnFrameEnd()
	{
		PrepareFrameAPI();

		for (unsigned int i = 0; i < RenderQueue.size(); i++) {
			if (RenderQueue[i]->InUse)
				PrepareCommandlists(i);
		}

		Profiler.StartProfile("Flush total");
		for (unsigned int i = 0; i < RenderQueue.size(); i++) {
			if (RenderQueue[i]->InUse)
				FlushRenderQueue(i);
		}
		Profiler.EndProfile("Flush total");

		return OnFrameEndAPI();
	}

/**
* Presents the backbuffer on screen
*/
	bool RDevice::Present()
	{
		RTools::TweakBar.Draw();

		Profiler.StartProfile("Present");
		bool r = PresentAPI();
		Profiler.EndProfile("Present");
		Profiler.EndProfile("Frame");

		std::string profilerOutput;

		Profiler.EndFrame(profilerOutput);
		return r;
	}

/**
 * Renders the given pipeline-state
 */
	bool RDevice::DrawPipelineState(const struct RPipelineState &state)
	{
		StateMachine.SetFromPipelineState(&state);

#ifndef PUBLIC_RELEASE
		// Do some safety checks
		if (StateMachine.GetCurrentState().VertexBuffers[0]) {
			size_t bufferSize = StateMachine.GetCurrentState().VertexBuffers[0]->GetSizeInBytes();
			size_t numBufferElements =
					bufferSize / StateMachine.GetCurrentState().VertexBuffers[0]->GetStructuredByteSize();
			assert(numBufferElements >= state.NumDrawElements + state.StartVertexOffset);
		}
#endif

		bool r = DrawPipelineStateAPI(state, StateMachine.GetChanges(), StateMachine);

		StateMachine.ResetChanges();

		return r;
	}

	bool RDevice::DrawPipelineState(const struct RPipelineState &state, const RStateMachine::ChangesStruct &changes,
									RStateMachine &stateMachine)
	{
		return DrawPipelineStateAPI(state, changes, stateMachine);
	}

/**
 * Renders an array of pipeline-states
 */
	bool RDevice::DrawPipelineStates(struct RPipelineState *const *stateArray, unsigned int numStates)
	{
		return DrawPipelineStatesAPI(stateArray, numStates);
	}

/** 
 * Returns a list of available display modes 
 */
	bool RDevice::GetDisplayModeList(std::vector<DisplayModeInfo> &modeList, bool includeSuperSampling)
	{
		return GetDisplayModeListAPI(modeList, includeSuperSampling);
	}

/** Returns the Counter of how many renderingstages we have gone through since the programs start
Functions like a frame-counter, but respects different rendering-stages */
	unsigned int RDevice::GetFrameCounter()
	{
		return FrameCounter;
	}

/**
* Puts the given pipeline-state into the renderingqueue, which is flushed at the end of the frame
*/
	bool RDevice::QueuePipelineState(const struct RPipelineState *state, RRenderQueueID queue)
	{
#ifndef PUBLIC_RELEASE
		if (RenderQueue.size() <= queue || !RenderQueue[queue]->InUse) {
			LogError() << "No renderqueue aquired on slot " << queue;
			return false;
		}
#endif

		QueuedDrawCallCounter++;
		RenderQueue[queue]->Queue.push_back(state);

		return true;
	}

/**
* Renders everything in the renderqueue
*/
	bool RDevice::FlushRenderQueue(RRenderQueueID queue)
	{
		RRenderQueue *q = RenderQueue[queue];

		if (!q->Name.empty())
			Profiler.StartProfile(q->Name);

		// Check if we have commandlists to do
		if (q->QueueCommandLists.empty()) {
			LEB(FlushQueueImmediate(queue))
		}
		else {
			LEB(FlushQueueCmdLists(queue))
		}

		if (!q->Name.empty())
			Profiler.EndProfile(q->Name);

		QueueCounter--;
		QueuedDrawCallCounter -= RenderQueue[queue]->Queue.size();
		RenderQueue[queue]->InUse = false;
		RenderQueue[queue]->Queue.clear();
		RenderQueue[queue]->Changes.clear();
		RenderQueue[queue]->QueueCommandLists.clear();

#ifndef PUBLIC_RELEASE
		RenderQueue[queue]->Sources.clear();
#endif

		return true;
	}

/**
* Draws the whole given queue on the main thread
*/
	bool RDevice::FlushQueueImmediate(RRenderQueueID queue)
	{
		RRenderQueue *q = RenderQueue[queue];

		// Sort the queue, in case it is wanted
		if (q->SortQueue)
			std::sort(q->Queue.begin(), q->Queue.end(), RPipelineState::KeyCompareSmall);

		// Just draw everything on the immediate context
		for (const RPipelineState *s : q->Queue) {
			DrawPipelineState(*s);
		}

		return true;
	}

/**
* Uses the generated commandlists of the given queue and draws them
*/
	bool RDevice::FlushQueueCmdLists(RRenderQueueID queue)
	{
		if (!RenderQueue[queue]->Queue.empty()) {
			// Execute the commandlists
			for (unsigned int i = 0; i < RenderQueue[queue]->QueueCommandLists.size(); i++) {
				// Wait for the commandlist to be available
				RenderQueue[queue]->QueueCommandListFutures[i].get();

				// Now execute
				RCommandList *c = RenderQueue[queue]->QueueCommandLists[i];
				LEB(c->ExecuteCommandList());
				REngine::ResourceCache->DeleteResource(c);
			}

			// Make sure we are back to default
			PrepareContextAPI(RTools::GetCurrentThreadId());
			StateMachine.Invalidate();
		}
		return true;
	}

/**
 * Registers a renderingqueue in the device. Registration will be cleared every frame, so you have to
 * get one every frame you want to use it */
	unsigned int RDevice::AcquireRenderQueue(bool sortable, const std::string &name)
	{
		QueueCounter++;

		for (unsigned int i = 0; i < RenderQueue.size(); i++) {
			if (!RenderQueue[i]->InUse) {
				RenderQueue[i]->InUse = true;
				RenderQueue[i]->SortQueue = sortable;
				RenderQueue[i]->Name = name;
				return i;
			}
		}

		// No free queue, add one
		RenderQueue.push_back(new RRenderQueue());

		RenderQueue.back()->InUse = true;
		RenderQueue.back()->SortQueue = sortable;
		RenderQueue.back()->Name = name;

		return RenderQueue.size() - 1;
	}

/** 
 * Fills the "changes"-vector of the given queue with values
 */
	void RDevice::ProcessRenderQueue(RRenderQueueID queue)
	{
		RRenderQueue &q = *RenderQueue[queue];

#ifdef NO_MULTITHREADED_RENDERING
		return;
#endif

		if (q.Queue.size() < MIN_STATES_FOR_THREADED_RENDER)
			return; // Don't do all this for really simple "immediate"-style queues

		if (!q.QueueCommandLists.empty()) // This should never happen as commandlists should be cleared after flushing!
			LogWarn() << "Processing queue with non-empty commandlist vector! This could cause memory leaks!";

		q.QueueCommandLists.resize(REngine::ThreadPool->getNumThreads());
		q.QueueCommandListFutures.resize(REngine::ThreadPool->getNumThreads());

#ifndef PUBLIC_RELEASE
		// Sanity // TODO: Remove, testing only.
		for (unsigned int i = 0; i < q.Queue.size() - 1; i++) {
			const RPipelineState *cs = q.Queue[i];
			const RPipelineState *ns = q.Queue[i + 1];
			/*if (cs->IDs.MainTexture == ns->IDs.MainTexture
				&& cs->IDs.VertexBuffer0 == ns->IDs.VertexBuffer0
				&& cs->StartIndexOffset == ns->StartIndexOffset
				&& cs->NumDrawElements == ns->NumDrawElements
				&& memcmp(&cs->Key, &ns->Key, sizeof(RPipelineState::Key)) == 0)
				__debugbreak();*/
		}
#endif

		for (unsigned int i = 0; i < REngine::ThreadPool->getNumThreads(); i++) {
			// Make sure we have a commandlist
			if (!q.QueueCommandLists[i]) {
				q.QueueCommandLists[i] = REngine::ResourceCache->CreateResource<RCommandList>();
				LEB(q.QueueCommandLists[i]->Init());
			}
		}

		// Make a new task for computing the changes of this queue
		q.ProcessedFuture = REngine::ThreadPool->enqueue(
				[this](unsigned int queue1) {
					RRenderQueue *q1p = RenderQueue[queue1];
					RRenderQueue &q1 = *q1p;

					// Sort the queue if wanted
					if (q1.SortQueue)
						std::sort(q1.Queue.begin(), q1.Queue.end(), RPipelineState::KeyCompareSmall);

					// Make sure the changes vector is big enough
					q1.Changes.resize(q1.Queue.size());

					// Make a new state machine, so we don't interfere with other threads
					RStateMachine sm;
					sm.Invalidate();

					// Create changes for each of the states
					for (unsigned int i = 0; i < q1.Queue.size(); i++) {
						// Enter our states and get the changes out
						sm.SetFromPipelineState(q1.Queue[i]);
						q1.Changes[i] = sm.GetChanges();

						// Bound everything, reset changes
						sm.ResetChanges();
					}
				}, queue);

		/*q.ProcessedFuture.wait();

		for (unsigned int i = 0; i < REngine::ThreadPool->getNumThreads(); i++)
		{
			q.QueueCommandListFutures[i].wait();
		}*/
	}

/**
* Prepares the renderqueues for rendering. Does the actual drawcalls.
*/
	bool RDevice::PrepareCommandlists(RRenderQueueID queue)
	{
		RRenderQueue &q1 = *RenderQueue[queue];

		if (q1.QueueCommandLists.empty())
			return true; // No multithreading for this queue

		// Make sure the queue was successfully processed
		RenderQueue[queue]->ProcessedFuture.get();

		if (RenderQueue[queue]->Changes.size() != RenderQueue[queue]->Queue.size()) {
			// This queue hasn't been processed yet

#ifndef PUBLIC_RELEASE
			LogWarn() <<
			"Flushing un-processed renderingqueue! This can be a performance-problem and should be done manually!";
#endif

			ProcessRenderQueue(queue);

			// Wait for the task to be done..
			RenderQueue[queue]->ProcessedFuture.get();
		}

		// Threadfunc which draws states from the queue
		auto threadfunc = [this](unsigned int queue2, unsigned int threadIdx, unsigned int start, unsigned int num) {
			RRenderQueue &q2 = *RenderQueue[queue2];

			// Make sure we set all states on first drawcall
			if (num > 0)
				memset(&q2.Changes[start], 1, sizeof(RStateMachine::ChangesStruct));

			// Create a new state-machine for this thread
			RStateMachine stateMachine;

			// Set up the initial states for this context
			PrepareContextAPI(RTools::GetCurrentThreadId());

			for (unsigned int i = start; i < start + num; i++) {
				DrawPipelineState(*q2.Queue[i], q2.Changes[i], stateMachine);

				// TODO: DEBUG-CODE
				((RPipelineState *) q2.Queue[i])->Locked = false;
			}

			if (q2.QueueCommandLists.empty())
				__debugbreak();

			// Finalize threads commandlist
			LEB(q2.QueueCommandLists[threadIdx]->FinalizeCommandList());
		};

		std::vector<std::future<void>> cmdListFutures;
		for (unsigned int i = 0; i < REngine::ThreadPool->getNumThreads(); i++) {
			unsigned int num = q1.Queue.size() / REngine::ThreadPool->getNumThreads();
			unsigned int start = num * i;

			// Add all  of the remaining work to the last thread
			if (i + 1 == REngine::ThreadPool->getNumThreads())
				num = q1.Queue.size() - start;

			assert(!q1.Queue.empty());

			// Push to threadpool
			q1.QueueCommandListFutures[i] = std::move(REngine::ThreadPool->enqueue(threadfunc, queue, i, start, num));
		}

		return true;
	}

/**
 * Returns the number of queues in use
 */
	unsigned int RDevice::GetNumQueuesInUse()
	{
		return QueueCounter;
	}

/**
 * Returns the total number of pipeline-states registered in the queues
 */
	unsigned int RDevice::GetNumRegisteredDrawCalls()
	{
		return QueuedDrawCallCounter;
	}

/**
* Returns the current main output window
*/
	WindowHandle RDevice::GetOutputWindow()
	{
		return OutputWindow;
	}

/**
* Returns a list of all result the profiler has gathered 
*/
	std::vector<std::pair<std::string, RProfiler::RProfileResult>> RDevice::GetProfilerResults()
	{
		return Profiler.GetAllProfileResults();
	}

/** Initializes the tweak-bar with values */
	void RDevice::AddTweakBarValues()
	{
		RTools::TweakBar.AddButton("renderer", "Reload Shaders", [](void *) { RTools::ReloadShaders(); });
		RTools::TweakBar.AddBoolRW("renderer", &DoDrawcalls, "Drawcalls");
	}

	/**
	 * Returns the resolution needed for the given window
	 */
	int2 RDevice::GetWindowResolution(WindowHandle hWnd)
	{
		return GetWindowResolutionAPI(hWnd);
	}
}