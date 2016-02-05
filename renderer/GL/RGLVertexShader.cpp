#include "pch.h"
#include "RGLVertexShader.h"

#ifdef RND_GL
using namespace RAPI;


/**
* Loads the given shader
*/
bool RGLVertexShader::LoadShaderAPI()
{
	return CompileShaderAPI(EShaderType::ST_VERTEX);
}

#endif