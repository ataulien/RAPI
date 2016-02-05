#include "pch.h"
#include "RGLPixelShader.h"

#ifdef RND_GL
using namespace RAPI;

/**
* Loads the given shader
*/
bool RGLPixelShader::LoadShaderAPI()
{
	return CompileShaderAPI(EShaderType::ST_PIXEL);
}

#endif