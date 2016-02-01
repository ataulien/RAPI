#include "pch.h"
#include "RGLDevice.h"
#include "RGLUtils.h"

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef RND_GL

using namespace RAPI;

bool RGLDevice::CreateDeviceAPI()
{
    RTools::GLExtMgr &ext = RTools::GLExtMgr::getSingleton();

    return true;
}

bool RGLDevice::SetWindowAPI()
{
#ifdef WIN32
    // FIXME: Cast
    HDC deviceContext = GetDC(static_cast<HWND>(OutputWindow));
    DeviceContext = static_cast<void*>(deviceContext);
    HGLRC renderContext;
#endif

    static PIXELFORMATDESCRIPTOR pfd = {
            sizeof(PIXELFORMATDESCRIPTOR),  // Size Of This Pixel Format Descriptor
            1,                                // Version Number
            PFD_DRAW_TO_WINDOW |            // Format Must Support Window
            PFD_SUPPORT_OPENGL |            // Format Must Support OpenGL
            PFD_DOUBLEBUFFER,                // Must Support Double Buffering
            PFD_TYPE_RGBA,                    // Request An RGBA Format
            32,                                // Select Our Color Depth
            0, 0, 0, 0, 0, 0,                // Color Bits Ignored
            0,                                // No Alpha Buffer
            0,                                // Shift Bit Ignored
            0,                                // No Accumulation Buffer
            0, 0, 0, 0,                        // Accumulation Bits Ignored
            16,                                // 16Bit Z-Buffer (Depth Buffer)
            0,                                // No Stencil Buffer
            0,                                // No Auxiliary Buffer
            PFD_MAIN_PLANE,                    // Main Drawing Layer
            0,                                // Reserved
            0, 0, 0                            // Layer Masks Ignored
    };

    int32_t pixelformat = 0;
    if (!(pixelformat = ChoosePixelFormat(deviceContext, &pfd)))
    {
        MessageBoxA(NULL, "Could not find suitable pixel format.", "Error!", MB_OK | MB_ICONERROR);
        return 1;
    }

    if (!SetPixelFormat(deviceContext, pixelformat, &pfd))
    {
        MessageBoxA(NULL, "Could not find set pixel format.", "Error!", MB_OK | MB_ICONERROR);
        return 1;
    }

    if (!(renderContext = wglCreateContext(deviceContext)))
    {
        MessageBoxA(NULL, "Could not create OpenGL context.", "Error!", MB_OK | MB_ICONERROR);
        return 1;
    }
    RenderContext = static_cast<void*>(renderContext);

    if (!wglMakeCurrent(deviceContext, renderContext))
    {
        MessageBoxA(NULL, "Could not activate render context.", "Error!", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Init first viewport
    int2 windowSize = GetWindowResolutionAPI(OutputWindow);
    glViewport(0, 0, windowSize.x, windowSize.y);

    glClearColor(0.0f, 0.5f, 0.5f,0.0f);

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
    SwapBuffers(static_cast<HDC>(DeviceContext));
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
int2 RGLDevice::GetWindowResolutionAPI(WindowHandle hWnd)
{
#ifdef WIN32
    RECT r;
    GetClientRect(static_cast<HWND>(hWnd), &r);
    return int2(r.right-r.left, r.bottom-r.top);
#else
    // FIXME
    return int2(800,600);
#endif
}
#endif