#pragma once
#include "RBaseShader.h"

namespace RAPI
{
#ifdef RND_D3D11

    class RD3D11Shader :
        public RBaseShader
    {
    public:
        RD3D11Shader();
        ~RD3D11Shader();

        /**
         * Loads the given shader
         */
        IUnknown* CompileShaderAPI(const std::string& entryPoint, const std::string& shaderModel, EShaderType shaderType);

        /**
         * Getters
         */
        ID3DBlob* GetShaderData(){return ShaderData;}

    private:
        /**
        * Find and compile the specified shader
        */
        HRESULT CompileShaderFromFile(const CHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut, const std::vector<std::string>& makros);

        // The blob holding the shaderdata
        ID3DBlob* ShaderData;
    };

#endif
}