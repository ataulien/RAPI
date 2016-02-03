#pragma once
#include "pch.h"
#include "RResource.h"

namespace RAPI
{
// Layed out for D3D11
	struct INPUT_ELEMENT_DESC
	{
		const char *SemanticName;
		unsigned int SemanticIndex;
		RAPI::EFormat Format;
		unsigned int InputSlot;
		unsigned int AlignedByteOffset;
		RAPI::EInputClassification InputSlotClass;
		unsigned int InstanceDataStepRate;
	};

	class RBaseInputLayout : public RResource
	{
	public:
		RBaseInputLayout();

		virtual ~RBaseInputLayout();

	protected:
		// Descriptor for the input layout
		INPUT_ELEMENT_DESC *InputElementDesc;
		unsigned int NumInputDescElements;
	};
}
