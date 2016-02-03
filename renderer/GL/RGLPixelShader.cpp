#include "pch.h"
#include "RGLPixelShader.h"

using namespace RAPI;

/**
* Loads the given shader
*/
bool RGLPixelShader::LoadShaderAPI()
{
	return CompileShaderAPI(EShaderType::ST_PIXEL);
}