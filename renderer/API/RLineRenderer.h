#pragma once
#include "pch.h"
#include "RBaseInputLayout.h"
#include "Types.h"

namespace RAPI
{

/** Structure to push line-vertices to the LineRenderer */
	struct LineVertex
	{
		static const INPUT_ELEMENT_DESC INPUT_LAYOUT_DESC[2];

		LineVertex()
		{ }

		// TODO
		/*LineVertex(const RAPI::float3& position, RAPI::RDWORD color = 0xFFFFFFFF)
		{
			Position = RAPI::float4(position.x, position.y, position.z, 1.0f);
			Color = RAPI::float4::FromColor(color);
		}*/

		LineVertex(const RAPI::float3 &position, const RAPI::float4 &color, float zScale = 1.0f)
		{
			Position = RAPI::float4(position.x, position.y, position.z, 1.0f);
			Position.w = zScale;
			Color = color;
		}


		RAPI::float4 Position;
		RAPI::float4 Color;
	};

	__declspec(selectany) const INPUT_ELEMENT_DESC LineVertex::INPUT_LAYOUT_DESC[2] =
			{
					{"POSITION", 0, RAPI::FORMAT_R32G32B32A32_FLOAT, 0, 0xFFFFFFFF, RAPI::INPUT_PER_VERTEX_DATA, 0},
					{"DIFFUSE",  0, RAPI::FORMAT_R32G32B32A32_FLOAT, 0, 0xFFFFFFFF, RAPI::INPUT_PER_VERTEX_DATA, 0},
			};


	class RBuffer;

	struct RPipelineState;

	class RLineRenderer
	{
	public:
		RLineRenderer(void);

		virtual ~RLineRenderer(void);

		/** Adds a line to the list */
		bool AddLine(const LineVertex &v1, const LineVertex &v2);

		/** Flushes the cached lines. Should be called only once per frame! */
		bool Flush(const RAPI::Matrix &viewProj);

		/** Clears the line cache */
		bool ClearCache();

		/** Adds a point locator to the renderlist */
		void AddPointLocator(const RAPI::float3 &location, float size = 1,
							 const RAPI::float4 &color = RAPI::float4(1, 1, 1, 1));

		/** Adds a plane to the renderlist */
		void AddPlane(const RAPI::float4 &plane, const RAPI::float3 &origin, float size = 1,
					  const RAPI::float4 &color = RAPI::float4(1, 1, 1, 1));

		/** Adds a ring to the renderlist */
		void AddRingZ(const RAPI::float3 &location, float size = 1.0f,
					  const RAPI::float4 &color = RAPI::float4(1, 1, 1, 1), int res = 32);

		/** Adds an AABB-Box to the renderlist */
		void AddAABB(const RAPI::float3 &location, float halfSize,
					 const RAPI::float4 &color = RAPI::float4(1, 1, 1, 1));

		void AddAABB(const RAPI::float3 &location, const RAPI::float3 &halfSize,
					 const RAPI::float4 &color = RAPI::float4(1, 1, 1, 1));

		void AddAABBMinMax(const RAPI::float3 &min, const RAPI::float3 &max,
						   const RAPI::float4 &color = RAPI::float4(1, 1, 1, 1));

		/** Adds a triangle to the renderlist */
		void AddTriangle(const RAPI::float3 &t0, const RAPI::float3 &t1, const RAPI::float3 &t2,
						 const RAPI::float4 &color = RAPI::float4(1, 1, 1, 1));

		/** Plots a vector of floats */
		void PlotNumbers(const std::vector<float> &values, const RAPI::float3 &location, const RAPI::float3 &direction,
						 float distance, float heightScale, const RAPI::float4 &color = RAPI::float4(1, 1, 1, 1));

	protected:

		struct LineConstantBuffer
		{
			RAPI::Matrix M_ViewProj;
		};

		/** Initializes the buffers and states */
		bool InitResources();

		// Cache since last flush
		std::vector<LineVertex> LineCache;

		// State to draw the line with
		RPipelineState *LinePipelineState;

		// Dynamic buffer for storing the line information
		RBuffer *LineBuffer;

		// Constantbuffer for the matrices
		RBuffer *LineCB;

		// The frame when the last call to flush() happened
		unsigned int LastFrameFlushed;
	};

}