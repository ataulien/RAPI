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
		/*LineVertex(const RAPI::RFloat3& position, RAPI::RDWORD color = 0xFFFFFFFF)
		{
			Position = RAPI::RFloat4(position.x, position.y, position.z, 1.0f);
			Color = RAPI::RFloat4::FromColor(color);
		}*/

		LineVertex(const RAPI::RFloat3 &position, const RAPI::RFloat4 &color, float zScale = 1.0f)
		{
			Position = RAPI::RFloat4(position.x, position.y, position.z, 1.0f);
			Position.w = zScale;
			Color = color;
		}


		RAPI::RFloat4 Position;
		RAPI::RFloat4 Color;
	};

	SELECTANY const INPUT_ELEMENT_DESC LineVertex::INPUT_LAYOUT_DESC[2] =
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
		bool Flush(const RAPI::RMatrix &viewProj);

		/** Clears the line cache */
		bool ClearCache();

		/** Adds a point locator to the renderlist */
		void AddPointLocator(const RAPI::RFloat3 &location, float size = 1,
							 const RAPI::RFloat4 &color = RAPI::RFloat4(1, 1, 1, 1));

		/** Adds a plane to the renderlist */
		void AddPlane(const RAPI::RFloat4 &plane, const RAPI::RFloat3 &origin, float size = 1,
					  const RAPI::RFloat4 &color = RAPI::RFloat4(1, 1, 1, 1));

		/** Adds a ring to the renderlist */
		void AddRingZ(const RAPI::RFloat3 &location, float size = 1.0f,
					  const RAPI::RFloat4 &color = RAPI::RFloat4(1, 1, 1, 1), int res = 32);

		/** Adds an AABB-Box to the renderlist */
		void AddAABB(const RAPI::RFloat3 &location, float halfSize,
					 const RAPI::RFloat4 &color = RAPI::RFloat4(1, 1, 1, 1));

		void AddAABB(const RAPI::RFloat3 &location, const RAPI::RFloat3 &halfSize,
					 const RAPI::RFloat4 &color = RAPI::RFloat4(1, 1, 1, 1));

		void AddAABBMinMax(const RAPI::RFloat3 &min, const RAPI::RFloat3 &max,
						   const RAPI::RFloat4 &color = RAPI::RFloat4(1, 1, 1, 1));

		/** Adds a triangle to the renderlist */
		void AddTriangle(const RAPI::RFloat3 &t0, const RAPI::RFloat3 &t1, const RAPI::RFloat3 &t2,
						 const RAPI::RFloat4 &color = RAPI::RFloat4(1, 1, 1, 1));

		/** Plots a vector of floats */
		void PlotNumbers(const std::vector<float> &values, const RAPI::RFloat3 &location, const RAPI::RFloat3 &direction,
						 float distance, float heightScale, const RAPI::RFloat4 &color = RAPI::RFloat4(1, 1, 1, 1));

	protected:

		struct LineConstantBuffer
		{
			RAPI::RMatrix M_ViewProj;
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