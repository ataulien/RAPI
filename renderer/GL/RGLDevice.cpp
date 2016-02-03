#include "pch.h"
#include "RGLDevice.h"
#include "Logger.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef RND_GL

using namespace RAPI;

bool RGLDevice::CreateDeviceAPI()
{
    return true;
}

bool RGLDevice::SetWindowAPI()
{
	// Make the window's context current
	glfwMakeContextCurrent(OutputWindow);

    // Init first viewport
    RInt2 windowSize = GetWindowResolutionAPI(OutputWindow);
    glViewport(0, 0, windowSize.x, windowSize.y);

    glClearColor(0.0f, 0.5f, 0.5f,0.0f);

	GLenum err = glewInit();
	if (GLEW_OK != err)
		LogError() << "Failed to init GLEW";
    return true;
}

bool RGLDevice::OnResizeAPI()
{
    return false;
}

bool RGLDevice::OnFrameStartAPI()
{
    glClearColor(0.0f, 0.5f, 1.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return true;
}

bool RGLDevice::PrepareFrameAPI()
{
    return false;
}

bool RGLDevice::OnFrameEndAPI()
{

    return true;
}

bool RGLDevice::PresentAPI()
{
	glfwSwapBuffers(OutputWindow);
    return true;
}

bool RGLDevice::DrawPipelineStateAPI(const struct RPipelineState &state,
                                     const RStateMachine::ChangesStruct &changes, RStateMachine &stateMachine)
{
    return false;
}

bool RGLDevice::DrawPipelineStatesAPI(struct RPipelineState *const *stateArray, unsigned int numStates)
{
    return false;
}

bool RGLDevice::RegisterThreadAPI(uint32_t threadID)
{
    return false;
}

bool RGLDevice::CreateCommandListForThreadAPI(uint32_t threadID)
{
    return false;
}

bool RGLDevice::PrepareContextAPI(unsigned int threadId)
{
    return false;
}

bool RGLDevice::GetDisplayModeListAPI(std::vector<DisplayModeInfo> &modeList, bool includeSuperSampling)
{
    return false;
}

/**
 * Returns the resolution needed for the given window
 */
RInt2 RGLDevice::GetWindowResolutionAPI(WindowHandle hWnd)
{
	RInt2 s;
	glfwGetFramebufferSize(hWnd, &s.x, &s.y);

    return s;
}
#endif