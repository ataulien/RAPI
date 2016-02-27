#pragma once

#include "Types.h"

namespace RAPI
{

/**
* Global namespace of REngine-Objects
*/
	class RDynamicBufferCache;

	class RResourceCache;

	class RDevice;

	class RThreadPool;
	namespace REngine
	{
		/**
		 * Initializes these objects
		 */
		bool InitializeEngine();

		/**
		 * Destroys these objects
		 */
		void UninitializeEngine();

		extern RDevice *RenderingDevice;
		extern RResourceCache *ResourceCache;
		extern RDynamicBufferCache *DynamicBufferCache;
		extern RThreadPool *ThreadPool;
	}
}