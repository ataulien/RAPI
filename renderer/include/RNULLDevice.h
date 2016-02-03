#pragma once
#include "RBaseDevice.h"

namespace RAPI
{
	class RNULLDevice : public RBaseDevice
	{
	public:
		/**
         * Creates the renderingdevice for the set API
         */
		bool CreateDeviceAPI(){return true;}

		/**
         * Sets the output window for this device
         */
		bool SetWindowAPI(){return true;}

		/**
         * Called when the output-window has been resized
         */
		bool OnResizeAPI(){return true;}

		/**
        * Called on frame-start
        */
		bool OnFrameStartAPI(){return true;}

		/**
         * Prepares a frame for rendering. Called between frame start and end
         */
		bool PrepareFrameAPI(){return true;}

		/**
        * Called on End-start
        */
		bool OnFrameEndAPI(){return true;}

		/**
        * Presents the backbuffer on screen
        */
		bool PresentAPI(){return true;}

		/**
         * Renders the given pipeline-state
         */
		bool DrawPipelineStateAPI(const struct RPipelineState &state, const RStateMachine::ChangesStruct &changes,
								  RStateMachine &stateMachine){return true;}

		/**
         * Renders an array of pipeline-states
         */
		bool DrawPipelineStatesAPI(struct RPipelineState *const *stateArray, unsigned int numStates){return true;}

		/**
         * Registers a thread in the renderer. Creates a resources like a deferred context.
         */
		bool RegisterThreadAPI(uint32_t threadID){return true;}

		/**
         * Creates the commandlist for the given threadID
         */
		bool CreateCommandListForThreadAPI(uint32_t threadID){return true;}

		/**
		 * Returns a list of available display modes
		 */
		bool GetDisplayModeListAPI(std::vector<DisplayModeInfo> &modeList, bool includeSuperSampling = false){}

		/**
        * Prepares the renderingcontext on the given thread for drawing
        */
		bool PrepareContextAPI(unsigned int threadId){return true;}
	};
}