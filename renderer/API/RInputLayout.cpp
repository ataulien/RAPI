#include "pch.h"
#include "RInputLayout.h"
#include "Logger.h"

namespace RAPI
{

	RInputLayout::RInputLayout()
	{
		InputElementDesc = nullptr;
	}


	RInputLayout::~RInputLayout()
	{
		delete[] InputElementDesc;
	}

/**
* Creates the inputlayout using the given input decleration
*/
	bool RInputLayout::CreateInputLayout(RVertexShader *vertexshadertemplate,
										 const INPUT_ELEMENT_DESC *inputElementDesc, unsigned int numInputDescElements)
	{
		if (InputElementDesc) {
			LogWarn() << "Can't initialize the same InputLayout twice!";
			return false;
		}

		NumInputDescElements = numInputDescElements;
		InputElementDesc = new INPUT_ELEMENT_DESC[numInputDescElements];
		memcpy(InputElementDesc, inputElementDesc, sizeof(INPUT_ELEMENT_DESC) * numInputDescElements);

		return CreateInputLayoutAPI(vertexshadertemplate);
	}

}