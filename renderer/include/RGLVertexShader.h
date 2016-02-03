#pragma once
#include "RGLShader.h"

#ifdef RND_GL
namespace RAPI
{
	class RGLVertexShader : public RGLShader
	{
	public:
		/**
		* Loads the given shader
		*/
		bool LoadShaderAPI(){return true;}

	};
}
#endif