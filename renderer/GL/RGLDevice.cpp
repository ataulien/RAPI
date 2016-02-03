#include "pch.h"
#include "RGLDevice.h"
#include "Logger.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "RBuffer.h"
#include "RPixelShader.h"
#include "RVertexShader.h"

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
	CheckGlError();

    // Init first viewport
    RInt2 windowSize = GetWindowResolutionAPI(OutputWindow);
    glViewport(0, 0, windowSize.x, windowSize.y);
	CheckGlError();

    glClearColor(0.0f, 0.5f, 0.5f,0.0f);
	CheckGlError();

	// start GLEW extension handler
	glewExperimental = GL_TRUE;

	GLenum err = glewInit();
	if (GLEW_OK != err)
		LogError() << "Failed to init GLEW";

	CheckGlError();

	// get version info
	const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString (GL_VERSION); // version as a string
	LogInfo() << "Renderer " << renderer;
	LogInfo() << "OpenGL version supported " << version;

	CheckGlError();

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

/**
* Binds the resources of the given pipeline state
*/
bool RGLDevice::BindPipelineState(const RPipelineState& state, const RStateMachine::ChangesStruct& changes, RStateMachine& stateMachine)
{
	stateMachine.SetFromPipelineState(&state, changes);
	const RPipelineStateFull& fs = stateMachine.GetCurrentState();
	std::vector<RGLShader*> shaders;

	//if(changes.PrimitiveType)
	//	context->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)state.IDs.PrimitiveType);

	//if(changes.RasterizerState && fs.RasterizerState)
	//	context->RSSetState(fs.RasterizerState->GetState());

	//if(changes.BlendState && fs.BlendState)
	//	context->OMSetBlendState(fs.BlendState->GetState(), (float *)&RFloat4(0.0f,0.0f,0.0f,0.0f), 0xFFFFFFFF);

	//if(changes.DepthStencilState && fs.DepthStencilState)
	//	context->OMSetDepthStencilState(fs.DepthStencilState->GetState(), 0);

	//if(changes.SamplerState && fs.SamplerState)
	//	context->PSSetSamplers(0, 1, fs.SamplerState->GetStatePtr());

	for(int i=0;i<2;i++)
	{
		if(changes.VertexBuffers[i])
		{
			if(fs.VertexBuffers[i])
			{
				unsigned int offsets[] = { 0 };
				unsigned int strides[] = { fs.VertexBuffers[i]->GetStructuredByteSize() };

				// Need to update the VAO of this to get the vertexlayout into the buffer
				fs.VertexBuffers[i]->UpdateVAO(fs.InputLayout);
				glBindVertexArray(fs.VertexBuffers[i]->GetVertexArrayObjectAPI()); 
				glBindBuffer(GL_ARRAY_BUFFER, fs.VertexBuffers[i]->GetVertexBufferObjectAPI());
				CheckGlError();
			}
			else
			{
				//unsigned int offsets[] = { 0 };
				//unsigned int strides[] = { 0 };
				//glBindVertexArray(0);
				//CheckGlError();
			}
		}
	}

	//if(changes.IndexBuffer && fs.IndexBuffer)
	//	context->IASetIndexBuffer(fs.IndexBuffer->GetBuffer(), 
	//		fs.IndexBuffer->GetStructuredByteSize() == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);

	if(changes.PixelShader && fs.PixelShader)
		shaders.push_back(fs.PixelShader);

	if(changes.VertexShader && fs.VertexShader)
		shaders.push_back(fs.VertexShader);

	// Link shaders/get a program from cache
	if(!shaders.empty())
	{
		glUseProgram(shaders[0]->LinkShaderObjectAPI(shaders));
		CheckGlError();
	}
	// TODO: Do this for all shader stages
	// TODO: Structured buffers use the same registers as textures. A change of them will
	// not affect the changed state of the textures.
	//if(changes.MainTexture)
	//{
	//	for(unsigned int i=0;i<fs.Textures[EShaderType::ST_PIXEL].size();i++)
	//		if (!fs.Textures[EShaderType::ST_PIXEL].empty())
	//		{
	//			ID3D11ShaderResourceView* srv = fs.Textures[EShaderType::ST_PIXEL][i] ? 
	//				fs.Textures[EShaderType::ST_PIXEL][i]->GetShaderResourceView()
	//				: nullptr;
	//			context->PSSetShaderResources(i, 1, &srv);
	//		}
	//}


	//if(changes.ConstantBuffers[EShaderType::ST_VERTEX])
	//{
	//	for(unsigned int j=0;j<fs.ConstantBuffers[EShaderType::ST_VERTEX].size();j++)
	//	{
	//		if(fs.ConstantBuffers[EShaderType::ST_VERTEX][j])
	//		{
	//			context->VSSetConstantBuffers(j, 1, fs.ConstantBuffers[EShaderType::ST_VERTEX][j]->GetBufferPtr());
	//		}
	//	}
	//}

	//if(changes.ConstantBuffers[EShaderType::ST_PIXEL])
	//{
	//	for(unsigned int j=0;j<fs.ConstantBuffers[EShaderType::ST_PIXEL].size();j++)
	//	{
	//		if(fs.ConstantBuffers[EShaderType::ST_PIXEL][j])
	//		{
	//			context->PSSetConstantBuffers(j, 1, fs.ConstantBuffers[EShaderType::ST_PIXEL][j]->GetBufferPtr());
	//		}
	//	}
	//}

	//if(changes.StructuredBuffers[EShaderType::ST_VERTEX])
	//{
	//	for(unsigned int j=0;j<fs.StructuredBuffers[EShaderType::ST_VERTEX].size();j++)
	//	{
	//		if(fs.StructuredBuffers[EShaderType::ST_VERTEX][j])
	//		{
	//			context->VSSetShaderResources(j, 1, fs.StructuredBuffers[EShaderType::ST_VERTEX][j]->GetBufferSRVPtr());
	//		}
	//	}
	//}

	//if(changes.Viewport && fs.Viewport)
	//{
	//	context->RSSetViewports(1, (D3D11_VIEWPORT*)&fs.Viewport->GetViewportInfo());
	//}

	return true;
}

bool RGLDevice::DrawPipelineStateAPI(const struct RPipelineState &state,
                                     const RStateMachine::ChangesStruct &changes, RStateMachine &stateMachine)
{
	// Bind everything
	if(DoDrawcalls)
	{
		BindPipelineState(state, changes, stateMachine);
		CheckGlError();

		// Perform drawcall
		switch(state.IDs.DrawFunctionID)
		{
		case EDrawCallType::DCT_Draw:
			glDrawArrays(GL_TRIANGLES, 0, state.NumDrawElements);
			break;

		case EDrawCallType::DCT_DrawIndexed:
			//context->DrawIndexed(state.NumDrawElements, state.StartIndexOffset, state.StartVertexOffset);
			break;

		case EDrawCallType::DCT_DrawIndexedInstanced:
			//context->DrawIndexedInstanced(state.NumDrawElements, state.NumInstances, state.StartIndexOffset, state.StartVertexOffset, state.StartInstanceOffset);
			break;
		}

		CheckGlError();
	}
    return true;
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