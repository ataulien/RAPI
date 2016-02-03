#include "pch.h"
#include "RGLVertexShader.h"

using namespace RAPI;

/**
* Loads the given shader
*/
bool RGLVertexShader::LoadShaderAPI()
{
	return CompileShaderAPI(EShaderType::ST_VERTEX);
}