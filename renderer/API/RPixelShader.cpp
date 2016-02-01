#include "pch.h"
#include "RPixelShader.h"
#include "Logger.h"

namespace RAPI
{

	RPixelShader::RPixelShader()
	{
	}


	RPixelShader::~RPixelShader()
	{
	}

/**
* Loads the given shader
*/
	bool RPixelShader::LoadShader(const std::string &file, const std::vector<std::vector<std::string>> &definitions)
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

	bool RPixelShader::LoadShaderFromString(const std::string &shadercode,
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
	bool RPixelShader::ReloadShader()
	{
		return LoadShaderAPI();
	}
}