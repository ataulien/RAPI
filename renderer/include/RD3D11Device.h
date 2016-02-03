#pragma once
#include "RBaseDevice.h"

#ifdef RND_D3D11

namespace RAPI
{
	class RTexture;

	class RD3D11Device :
			public RBaseDevice
	{
	public:
		RD3D11Device();

		~RD3D11Device();

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
		bool RegisterThreadAPI(UINT threadID);

		/**
         * Creates the commandlist for the given threadID
         */
		bool CreateCommandListForThreadAPI(UINT threadID);

        /**
         * Returns the resolution needed for the given window
         */
		RInt2 GetWindowResolutionAPI(WindowHandle hWnd);

		/**
         * Getters
         */
		ID3D11Device *GetDevice()
		{ return Device; }

		ID3D11DeviceContext *GetThreadContext(UINT threadID);

		ID3D11DeviceContext *GetImmediateContext();

		/**
         * Returns a list of available display modes
         */
		bool GetDisplayModeListAPI(std::vector<DisplayModeInfo> &modeList, bool includeSuperSampling = false);

		/**
        * Prepares the renderingcontext on the given thread for drawing
        */
		bool PrepareContextAPI(unsigned int threadId);

	private:


		/**
         * Binds the resources of the given pipeline state
         */
		bool BindPipelineState(const RPipelineState &state, const RStateMachine::ChangesStruct &changes,
							   ID3D11DeviceContext *context, RStateMachine &stateMachine);

		/**
         * Initializes DXGI-Resources like the adapter we are rendering on
         */
		bool InitDXGI();

		/**
         * Creates a swapchain for the set output window
         */
		bool CreateSwapChain(bool createWindowed);

		/**
         * Executes all commandlists for all threads we have gotten so far
         */
		bool ExecuteDeferredCommandLists();

		// API-Specific rendering device
		ID3D11Device *Device;
		ID3D11DeviceContext *ImmediateContext;

		// DXGI-Instances
		IDXGIFactory *DXGIFactory;
		IDXGIAdapter *DXGIAdapter;
		IDXGISwapChain *DXGISwapChain;
		RTexture *Backbuffer;
		RTexture *DepthStencilBuffer;

		// Map of device contexts by threadID.
		std::unordered_map<UINT, std::pair<ID3D11DeviceContext *, std::vector<ID3D11CommandList *>>> ThreadContexts;
		std::mutex ThreadCommandListMutex; // Locked when changes of the threads commandlist arrays are done
	};
}
#endif