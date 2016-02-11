#include "pch.h"

#ifdef RND_D3D11
#include "RD3D11Shader.h"
#include "Logger.h"
#include "REngine.h"

#pragma comment(lib, "d3dx11.lib")

using namespace RAPI;

RD3D11Shader::RD3D11Shader()
{
	ShaderData = nullptr;
}


RD3D11Shader::~RD3D11Shader()
{
	SafeRelease(ShaderData);
}

//--------------------------------------------------------------------------------------
// Find and compile the specified shader
//--------------------------------------------------------------------------------------
HRESULT RD3D11Shader::CompileShaderFromFile(const CHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut, const std::vector<std::string>& makros)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	//dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
	dwShaderFlags |= 1;

	// Construct makros
	std::vector<D3D10_SHADER_MACRO> mvec;
	for each (const std::string& s in makros)
	{
		D3D10_SHADER_MACRO m = { s.c_str(), "" };
		mvec.push_back(m);

	}

	// Add the null-one to the end
	D3D10_SHADER_MACRO m = { nullptr, nullptr };
	mvec.push_back(m);

	ID3DBlob* pErrorBlob;

	if(!IsFromMemory)
	{
		hr = D3DX11CompileFromFileA(szFileName, &mvec[0], NULL, szEntryPoint, szShaderModel,
			dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	}
	else
	{
		hr = D3DX11CompileFromMemory(ShaderFile.c_str(), ShaderFile.size(), "ShaderFromMemory", &mvec[0], NULL, szEntryPoint, szShaderModel,
			dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	}

	if (FAILED(hr))
	{
		LogInfo() << "Shader compilation failed for: " << szFileName;
		if (pErrorBlob != NULL)
		{
			LogErrorBox() << (char*)pErrorBlob->GetBufferPointer();
			pErrorBlob->Release();
		}

		return hr;
	}
	if (pErrorBlob)
	{
		/*if(Engine->SwapchainCreated())
		Engine->GetConsole()->PostConsoleMessage((char*)pErrorBlob->GetBufferPointer());
		else
		LogWarnBox() << (char*)pErrorBlob->GetBufferPointer() << "nn (You can ignore the next error from Gothic about too small video memory!)";
		*/
		pErrorBlob->Release();
	}

	return S_OK;
}

/**
* Loads the given shader
*/
IUnknown* RD3D11Shader::CompileShaderAPI(const std::string & entryPoint, const std::string & shaderModel, EShaderType shaderType)
{
	HRESULT hr;

	// Compile shader
	std::vector<std::string> def;
	if(!Definitions.empty())
		def = Definitions[0]; // TODO: Go through all definitions

							  // Compile the shaderfile
	LE(CompileShaderFromFile(ShaderFile.c_str(), entryPoint.c_str(), shaderModel.c_str(), &ShaderData, def));
	if(hr != S_OK)
		return false;

	// Create the shader
	IUnknown* shader = nullptr;
	switch(shaderType)
	{
	case ST_VERTEX:
		LE(REngine::RenderingDevice->GetDevice()->CreateVertexShader(ShaderData->GetBufferPointer(),
			ShaderData->GetBufferSize(), NULL, (ID3D11VertexShader **)&shader));
		break;

	case ST_PIXEL:
		LE(REngine::RenderingDevice->GetDevice()->CreatePixelShader(ShaderData->GetBufferPointer(),
			ShaderData->GetBufferSize(), NULL, (ID3D11PixelShader **)&shader));
		break;

	case ST_GEOMETRY:
		LE(REngine::RenderingDevice->GetDevice()->CreateGeometryShader(ShaderData->GetBufferPointer(),
			ShaderData->GetBufferSize(), NULL, (ID3D11GeometryShader **)&shader));
		break;

	case ST_HULL:
		LE(REngine::RenderingDevice->GetDevice()->CreateHullShader(ShaderData->GetBufferPointer(),
			ShaderData->GetBufferSize(), NULL, (ID3D11HullShader **)&shader));
		break;

	case ST_DOMAIN:
		LE(REngine::RenderingDevice->GetDevice()->CreateDomainShader(ShaderData->GetBufferPointer(),
			ShaderData->GetBufferSize(), NULL, (ID3D11DomainShader **)&shader));
		break;
	default:
		LogWarn() << "Unknown shadertype: " << shaderType;
	}

	return shader;
}
#endif
