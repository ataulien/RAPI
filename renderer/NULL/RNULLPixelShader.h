#pragma once
#include "RNULLShader.h"

namespace RAPI
{
	class RNULLPixelShader : public RNULLShader
	{
	public:
		/**
		* Loads the given shader
		*/
		bool LoadShaderAPI(){return true;}
	};

}