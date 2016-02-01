#include "pch.h"
#include "REngine.h"
#include "RDevice.h"
#include "RResourceCache.h"
#include "RDynamicBufferCache.h"
#include "RThreadPool.h"
#include <thread>
#include <assert.h>
#include <math.h>

using namespace RAPI;

namespace RAPI
{

	RDevice *REngine::RenderingDevice;
	RResourceCache *REngine::ResourceCache;
	RDynamicBufferCache *REngine::DynamicBufferCache;
	RThreadPool *REngine::ThreadPool;

/** Simple struct to be initialized right after the program was loaded.
	Initializes these engine-resources to a defined value. */
	volatile struct _hlpObject
	{
		// Perform some startup-code
		_hlpObject()
		{
			REngine::RenderingDevice = nullptr;
			REngine::ResourceCache = nullptr;
			REngine::DynamicBufferCache = nullptr;
			REngine::ThreadPool = nullptr;
		}

		~_hlpObject()
		{
			// Clean exit?
			assert(!REngine::RenderingDevice);
			assert(!REngine::ResourceCache);
			assert(!REngine::DynamicBufferCache);
			assert(!REngine::ThreadPool);
		}
	} __hlpObj;

/**
 * Initializes these objects 
 */
	bool REngine::InitializeEngine()
	{
		REngine::RenderingDevice = new RDevice();
		REngine::ResourceCache = new RResourceCache();
		REngine::DynamicBufferCache = new RDynamicBufferCache();
		REngine::ThreadPool = new RThreadPool(std::thread::hardware_concurrency() * 2);

		return true;
	}

/**
 * Destroys these objects
 */
	void REngine::UninitializeEngine()
	{
		delete REngine::ResourceCache;
		delete REngine::RenderingDevice;
		delete REngine::DynamicBufferCache;
		delete REngine::ThreadPool;

		REngine::RenderingDevice = nullptr;
		REngine::ResourceCache = nullptr;
		REngine::DynamicBufferCache = nullptr;
		REngine::ThreadPool = nullptr;
	}
}