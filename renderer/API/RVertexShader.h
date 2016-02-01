#pragma once

#ifdef RND_D3D11
#include "RD3D11VertexShader.h"
#define RVSBASE_API RD3D11VertexShader
#elif defined(RND_NULL)
#include "RNULLVertexShader.h"
#define RVSBASE_API RNULLVertexShader
#elif defined(RND_GL)
#include "RGLVertexShader.h"
#define RVSBASE_API RGLVertexShader
#endif

namespace RAPI
{

	class RVertexShader :
			public RVSBASE_API
	{
	public:
		RVertexShader();

		~RVertexShader();

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