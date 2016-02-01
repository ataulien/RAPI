#include "pch.h"
#include "RVertexShader.h"
#include "Logger.h"

namespace RAPI
{
	RVertexShader::RVertexShader()
	{
	}


	RVertexShader::~RVertexShader()
	{
	}

/**
* Loads the given shader
*/
	bool RVertexShader::LoadShader(const std::string &file, const std::vector<std::vector<std::string>> &definitions)
	{
		LogInfo() << "Compilling shader: " << file;

		// Make sure this is only used once
		if (!ShaderFile.empty()) {
			LogWarn() << "Shader '" << ShaderFile << "' already loaded. Can't overwrite with: " << file <<
			". Create a new object!";
			return false;
		}

		ShaderFile = file;
		Definitions = definitions;
		IsFromMemory = false;

		return LoadShaderAPI();
	}

	bool RVertexShader::LoadShaderFromString(const std::string &shadercode,
											 const std::vector<std::vector<std::string>> &definitions)
	{
		LogInfo() << "Compilling shader: " << shadercode;

		// Make sure this is only used once
		if (!ShaderFile.empty()) {
			LogWarn() << "Shader '" << ShaderFile << "' already loaded. Can't overwrite with: " << shadercode <<
			". Create a new object!";
			return false;
		}

		ShaderFile = shadercode;
		Definitions = definitions;
		IsFromMemory = true;

		return LoadShaderAPI();
	}

/** Reloads the shader */
	bool RVertexShader::ReloadShader()
	{
		return LoadShaderAPI();
	}
}