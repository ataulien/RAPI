#pragma once

#include "RBaseInputLayout.h"

namespace RAPI
{
    class RNULLInputLayout : public RBaseInputLayout
    {
    public:
        /**
        * Creates the inputlayout using the given input decleration
        */
        bool CreateInputLayoutAPI(class RVertexShader *vertexshadertemplate){return true;}
    };

}