#pragma once

#ifdef RND_D3D11
#include "RD3D11PixelShader.h"
#define RPSBASE_API RD3D11PixelShader
#elif defined(RND_NULL)
#include "RNULLPixelShader.h"
#define RPSBASE_API RNULLPixelShader
#elif defined(RND_GL)
#include "RGLPixelShader.h"
#define RPSBASE_API RGLPixelShader
#endif


namespace RAPI
{
	class RPixelShader :
			public RPSBASE_API
	{
	public:
		RPixelShader();

		~RPixelShader();

		/**
		* Loads the given shader
		*/
		bool LoadShader(const std::string &file,
						const std::vector<std::vector<std::string>> &definitions = std::vector<std::vector<std::string>>());

		bool LoadShaderFromString(const std::string &shadercode,
								  const std::vector<std::vector<std::string>> &definitions = std::vector<std::vector<std::string>>());

		/** Reloads the shader */
		bool ReloadShader();
	};

}