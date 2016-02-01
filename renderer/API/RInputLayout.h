#pragma once
#include "pch.h"

#ifdef RND_D3D11
#include "RD3D11InputLayout.h"
#define RINPUTLAYOUTBASE_API RD3D11InputLayout
#elif defined(RND_NULL)
#include "RNULLInputLayout.h"
#define RINPUTLAYOUTBASE_API RNULLInputLayout
#elif defined(RND_GL)
#include "RGLInputLayout.h"
#define RINPUTLAYOUTBASE_API RGLInputLayout
#endif

namespace RAPI
{

	class RInputLayout :
			public RINPUTLAYOUTBASE_API
	{
	public:
		RInputLayout();

		~RInputLayout();

		/**
		* Creates the inputlayout using the given input decleration
		*/
		bool CreateInputLayout(class RVertexShader *vertexshadertemplate, const INPUT_ELEMENT_DESC *inputElementDesc,
							   unsigned int numInputDescElements);
	};

}
