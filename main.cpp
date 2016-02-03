#include "pch.h"
#include <iostream>
#include "REngine.h"
#include <GLFW\glfw3.h>
#include "RBuffer.h"
#include "REngine.h"
#include "RResourceCache.h"
#include "RStateMachine.h"
#include "RPixelShader.h"
#include "RVertexShader.h"
#include "RTools.h"
#include "RInputLayout.h"
#include "Logger.h"

using namespace std;
using namespace RAPI;

const char* vertex_shader =
"#version 150\n"
"in vec3 vp;"
"void main () {"
"  gl_Position = vec4 (vp, 1.0);"
"}";

const char* fragment_shader =
"#version 150\n"
"out vec4 frag_colour;"
"void main () {"
"  frag_colour = vec4 (0.5, 0.0, 0.5, 1.0);"
"}";

RPipelineState* g_Triangle = nullptr;


/** We pack most of Gothics FVF-formats into this vertex-struct */
struct SimpleVertex
{
	static const RAPI::INPUT_ELEMENT_DESC INPUT_LAYOUT_DESC[1]; 

	float Position[3];

};
__declspec(selectany) const RAPI::INPUT_ELEMENT_DESC SimpleVertex::INPUT_LAYOUT_DESC[1] =
{
	{ "POSITION", 0, RAPI::FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, RAPI::INPUT_PER_VERTEX_DATA, 0 },
};


void Update()
{
    REngine::RenderingDevice->OnFrameStart();

	RRenderQueueID queue = REngine::RenderingDevice->AcquireRenderQueue();
	REngine::RenderingDevice->QueuePipelineState(g_Triangle, queue);

	REngine::RenderingDevice->ProcessRenderQueue(queue);

    REngine::RenderingDevice->OnFrameEnd();

    REngine::RenderingDevice->Present();
}

void RunApp(GLFWwindow* window)
{
	RPixelShader* ps = RTools::LoadShaderFromString<RPixelShader>(fragment_shader, "simplePS");
	RBuffer* b = REngine::ResourceCache->CreateResource<RBuffer>();
	RStateMachine& sm = REngine::RenderingDevice->GetStateMachine();
	
	RVertexShader* vs = RTools::LoadShaderFromString<RVertexShader>(vertex_shader, "simpleVS");
	RInputLayout* inputLayout = RTools::CreateInputLayoutFor<SimpleVertex>(vs);

	SimpleVertex p[3] = { { 0.0f,  0.5f,  0.0f},
							{0.5f, -0.5f,  0.0f},
							{-0.5f, -0.5f,  0.0f} };

	b->Init(p, sizeof(p), sizeof(SimpleVertex));

	sm.SetVertexBuffer(0, b);
	sm.SetPixelShader(ps);
	sm.SetVertexShader(vs);
	sm.SetInputLayout(inputLayout);

	g_Triangle = sm.MakeDrawCall(3);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		Update();

		/* Poll for and process events */
		glfwPollEvents();	
	}

	REngine::ResourceCache->DeleteResource(b);
}

#ifdef WIN32
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
#else
int main(int argc, char** argv)
#endif
{
	Log::Clear();

	//Window wnd;
	//wnd.Create("--- Test ---", 20, 20, 1280, 720, hInstance, WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	GLFWwindow* wnd = glfwCreateWindow(1280, 720, "--- Test ---", nullptr, nullptr);



	if(!wnd)
	{
		glfwTerminate();
		return -1;
	}

	

	REngine::InitializeEngine();
	REngine::RenderingDevice->CreateDevice();
    REngine::RenderingDevice->SetWindow(wnd);
	cout << "Hello, World!" << endl;

    RunApp(wnd);

	REngine::UninitializeEngine();

	glfwTerminate();

	return 0;
}