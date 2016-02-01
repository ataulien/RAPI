#pragma once
#include "RBaseDevice.h"

#ifdef RND_GL
namespace RAPI
{
	class RGLDevice : public RBaseDevice
	{
	public:
		/**
         * Creates the renderingdevice for the set API
         */
		bool CreateDeviceAPI();

		/**
         * Sets the output window for this device
         */
		bool SetWindowAPI();

		/**
         * Called when the output-window has been resized
         */
		bool OnResizeAPI();

		/**
        * Called on frame-start
        */
		bool OnFrameStartAPI();

		/**
         * Prepares a frame for rendering. Called between frame start and end
         */
		bool PrepareFrameAPI();

		/**
        * Called on End-start
        */
		bool OnFrameEndAPI();

		/**
        * Presents the backbuffer on screen
        */
		bool PresentAPI();

		/**
         * Renders the given pipeline-state
         */
		bool DrawPipelineStateAPI(const struct RPipelineState &state, const RStateMachine::ChangesStruct &changes,
								  RStateMachine &stateMachine);

		/**
         * Renders an array of pipeline-states
         */
		bool DrawPipelineStatesAPI(struct RPipelineState *const *stateArray, unsigned int numStates);

		/**
         * Registers a thread in the renderer. Creates a resources like a deferred context.
         */
		bool RegisterThreadAPI(uint32_t threadID);

		/**
         * Creates the commandlist for the given threadID
         */
		bool CreateCommandListForThreadAPI(uint32_t threadID);

		/**
		 * Returns a list of available display modes
		 */
		bool GetDisplayModeListAPI(std::vector<DisplayModeInfo> &modeList, bool includeSuperSampling = false);

		/**
		* Returns the resolution needed for the given window
		*/
		int2 GetWindowResolutionAPI(WindowHandle hWnd);

		/**
        * Prepares the renderingcontext on the given thread for drawing
        */
		bool PrepareContextAPI(unsigned int threadId);

	private:
		// Current contexts
		void* DeviceContext;
		void* RenderContext;
	};
}
#endif