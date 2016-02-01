#pragma once

#include "RBaseInputLayout.h"

#ifdef RND_GL
namespace RAPI
{
    class RGLInputLayout : public RBaseInputLayout
    {
    public:
        /**
        * Creates the inputlayout using the given input decleration
        */
        bool CreateInputLayoutAPI(class RVertexShader *vertexshadertemplate){return true;}
    };

}
#endif