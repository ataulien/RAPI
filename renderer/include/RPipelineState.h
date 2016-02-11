#pragma once

#include <string.h>
#include "pch.h"
#include "Types.h"
#include "RResource.h"

#ifndef RAPI_MAX_NUM_SHADER_RESOURCES
#define RAPI_MAX_NUM_SHADER_RESOURCES 4
#endif

namespace RAPI
{

	struct RPipelineState : public RResource
	{
		RPipelineState()
		{
			memset(&Key, 0xFF, sizeof(Key));
			Locked = false;
		}

		~RPipelineState()
		{
		}

		// Draw order of the objects
		// Hint: You can swap these around to modify the actual draw-order.
		enum EDrawOrder
		{
			DO_OPAQUE,
			DO_ALPHA_TEST,
			DO_ALPHA_BLEND
		};

		// Struct to access the resource-ids
		struct IDStruct
		{
			// -- 4
			EPrimitiveType PrimitiveType : 4;

			// -- 32
			unsigned int RasterizerState : 8;
			unsigned int BlendState : 8;
			unsigned int DepthStencilState : 8;
			unsigned int SamplerState : 8;

			// -- 48
			unsigned int VertexBuffer0 : 16;
			unsigned int VertexBuffer1 : 16;
			unsigned int IndexBuffer : 16;

			// -- 29
			unsigned int PixelShader : 5;
			unsigned int VertexShader : 5;
			unsigned int GeometryShader : 5;
			unsigned int HullShader : 5;
			unsigned int DomainShader : 5;
			unsigned int InputLayout : 5;

			// -- 15
			unsigned int MainTexture : 15;

			// -- 7
			unsigned int DrawFunctionID : 3;

			// -- 16
			unsigned int ViewportID : 16;

			// -- 3
			EDrawOrder DrawOrder : 3;
			// Total: 32 + 48 + 29 + 15 + 3 + 16 = 144
		};

		struct KeyStruct
		{
			uint64_t Part[3];
		};

		union
		{
			IDStruct IDs;

			// Combined state-key
			KeyStruct Key;
		};

		/**
		 * Implementation if the <-operator
		 */
		static bool KeyCompare(const RPipelineState &a, const RPipelineState &b)
		{
			return a.Key.Part[0] < b.Key.Part[0] ||
				   (a.Key.Part[0] == b.Key.Part[0] && a.Key.Part[1] < b.Key.Part[1]) ||
				   (a.Key.Part[0] == b.Key.Part[0] && a.Key.Part[1] == b.Key.Part[1] && a.Key.Part[2] < b.Key.Part[2]);
		}

		static bool KeyComparePtr(const RPipelineState *a, const RPipelineState *b)
		{
			return KeyCompare(*a, *b);
		}

		static bool KeyCompareSmall(const RPipelineState *a, const RPipelineState *b)
		{
			return a->IDs.MainTexture + a->IDs.DrawOrder * 1000000 < b->IDs.MainTexture + b->IDs.DrawOrder * 1000000;
		}

		// Textures and constantbuffers
		std::array<class RBuffer *, RAPI_MAX_NUM_SHADER_RESOURCES> StructuredBuffers[EShaderType::ST_NUM_SHADER_TYPES];
		std::array<class RTexture *, RAPI_MAX_NUM_SHADER_RESOURCES> Textures[EShaderType::ST_NUM_SHADER_TYPES];
		std::array<class RBuffer *, RAPI_MAX_NUM_SHADER_RESOURCES> ConstantBuffers[EShaderType::ST_NUM_SHADER_TYPES];
		unsigned int NumDrawElements; // Vertices, indices...
		unsigned int StartVertexOffset;
		unsigned int StartIndexOffset;
		unsigned int StartInstanceOffset;
		unsigned int NumInstances;

		// TODO: Testing only, remove
		bool Locked;

		class GBaseDrawable *source;
	};
}