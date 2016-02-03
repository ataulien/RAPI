#pragma once
#include "RBaseInputLayout.h"

#ifdef RND_D3D11

namespace RAPI
{
	class RD3D11InputLayout :
		public RBaseInputLayout
	{
	public:
		RD3D11InputLayout();
		~RD3D11InputLayout();

		/**
		* Creates the inputlayout using the given input decleration
		*/
		bool CreateInputLayoutAPI(class RVertexShader* vertexshadertemplate);

		/** Getters */
		ID3D11InputLayout* GetInputLayout(){return InputLayout;}
	private:
		// The inputlayout object
		ID3D11InputLayout* InputLayout;
	};
}

#endif