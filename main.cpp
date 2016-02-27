#include "pch.h"

#if !(defined(WIN32) || defined(_WIN32))

int main(int argc, char** argv)
{
	return 0;
}

#else

#include <iostream>
#include <REngine.h>
#include <GLFW/glfw3.h>

#if defined(WIN32) || defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#include <RTools.h>
#include <RBuffer.h>
#include <REngine.h>
#include <RResourceCache.h>
#include <RStateMachine.h>
#include <RPixelShader.h>
#include <RVertexShader.h>
#include <RDevice.h>
#include <RInputLayout.h>
#include "logger.h"

using namespace std;
using namespace RAPI;

#ifdef RND_GL
const char* vertex_shader =
"#version 420\n"
"#extension GL_ARB_enhanced_layouts : enable\n"
"#extension GL_ARB_explicit_uniform_location : enable\n"
""
"layout (std140, binding = 0) uniform shader_data\n"
"{ \n "
"	vec4 V_Offset;\n "
"}; \n "
"\n "
"\n "
"in vec3 vp;\n"
"void main () {\n"
"	gl_Position = vec4 (vp + V_Offset.xyz, 1.0);\n"
"}\n";

const char* fragment_shader =
"#version 420\n"
"out vec4 frag_colour;"
"void main () {"
"  frag_colour = vec4 (0.5, 0.0, 0.5, 1.0);"
"}";
#else
const char* vertex_shader =
"cbuffer cb : register(b0)"
"{"
"float4 V_Offset;"
"};"
"float4 VSMain (float3 vp : POSITION) : SV_POSITION {"
"  return float4(vp + V_Offset.xyz, 1.0);"
"}";

const char* fragment_shader =
"float4 PSMain () : SV_TARGET {"
"  return float4 (0.5, 0.5, 0.0, 1.0);"
"}";
#endif



RPipelineState* g_Triangle = nullptr;
RBuffer* g_ConstantBuffer = nullptr;


/** We pack most of Gothics FVF-formats into this vertex-struct */
struct SimpleVertex
{
	static const RAPI::INPUT_ELEMENT_DESC INPUT_LAYOUT_DESC[1]; 

	float Position[3];

};

const RAPI::INPUT_ELEMENT_DESC SimpleVertex::INPUT_LAYOUT_DESC[1] =
{
	{ "POSITION", 0, RAPI::FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, RAPI::INPUT_PER_VERTEX_DATA, 0 },
};


void Update()
{
	static float s_t = 0;
	s_t += 0.01f;

	RFloat4 pos = RFloat4(sinf(s_t) * 0.1f, cosf(s_t) * 0.1f, 0, 0);
	g_ConstantBuffer->UpdateData(&pos);

	REngine::RenderingDevice->OnFrameStart();

	RRenderQueueID queue = REngine::RenderingDevice->AcquireRenderQueue();

	for(int i=0;i<1000;i++)
		REngine::RenderingDevice->QueuePipelineState(g_Triangle, queue);

	REngine::RenderingDevice->ProcessRenderQueue(queue);

	REngine::RenderingDevice->OnFrameEnd();

	REngine::RenderingDevice->Present();
}

void RunApp(GLFWwindow* window)
{
	RPixelShader* ps = RTools::LoadShaderFromString<RPixelShader>(fragment_shader, "simplePS");
	RBuffer* b = REngine::ResourceCache->CreateResource<RBuffer>();
	RBuffer* cb = REngine::ResourceCache->CreateResource<RBuffer>();
	RStateMachine& sm = REngine::RenderingDevice->GetStateMachine();

	RVertexShader* vs = RTools::LoadShaderFromString<RVertexShader>(vertex_shader, "simpleVS");
	RInputLayout* inputLayout = RTools::CreateInputLayoutFor<SimpleVertex>(vs);

	SimpleVertex p[3] = { { 0.0f,  0.5f,  0.5f},
	{0.5f, -0.5f,  0.5f},
	{-0.5f, -0.5f,  0.5f} };

	b->Init(p, sizeof(p), sizeof(SimpleVertex));
	cb->Init(nullptr, sizeof(RFloat4), sizeof(RFloat4), EBindFlags::B_CONSTANTBUFFER, EUsageFlags::U_DYNAMIC, ECPUAccessFlags::CA_WRITE);
	g_ConstantBuffer = cb;

	sm.SetVertexBuffer(0, b);
	sm.SetPixelShader(ps);
	sm.SetVertexShader(vs);
	sm.SetInputLayout(inputLayout);
	sm.SetConstantBuffer(0, cb, EShaderType::ST_VERTEX);

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

#if defined(WIN32) || defined(_WIN32)
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

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	GLFWwindow* wnd = glfwCreateWindow(1280, 720, "--- Test ---", nullptr, nullptr);



	if(!wnd)
	{
		glfwTerminate();
		return -1;
	}



	REngine::InitializeEngine();
	REngine::RenderingDevice->CreateDevice();

#ifdef RND_GL
	REngine::RenderingDevice->SetWindow(wnd);
#else
	REngine::RenderingDevice->SetWindow(glfwGetWin32Window(wnd));
#endif
	cout << "Hello, World!" << endl;

	RunApp(wnd);

	REngine::UninitializeEngine();

	glfwTerminate();

	return 0;
}
#endif