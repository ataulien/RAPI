#include <math.h>
#include "pch.h"
#include "RLineRenderer.h"
#include "RBuffer.h"
#include "REngine.h"
#include "RResourceCache.h"
#include "RStateMachine.h"
#include "RDevice.h"
#include "RTools.h"
#include "RViewport.h"
#include "RVertexShader.h"
#include "RPixelShader.h"
#include "REngine.h"

namespace RAPI
{

#ifndef RND_D3D11
#pragma warning Shaders for Lines hardcoded as HLSL. You may want to change that!
#endif

	const char *LINE_VERTEX_SHADER = "cbuffer Matrices_PerFrame : register( b0 )"
			"{"
			"	matrix M_ViewProj;	"
			"};"
			""
			"struct VS_INPUT"
			"{"
			"	float4 vPosition	: POSITION;"
			"	float4 vDiffuse		: DIFFUSE;"
			"};"
			""
			"struct VS_OUTPUT"
			"{"
			"	float4 vDiffuse			: TEXCOORD0;"
			"	float4 vPosition		: SV_POSITION;"
			"};"
			""
			"VS_OUTPUT VSMain( VS_INPUT Input )"
			"{"
			"	VS_OUTPUT Output;"
			""
			"	Output.vPosition = mul( float4(Input.vPosition.xyz,1), M_ViewProj);"
			"	Output.vDiffuse  = Input.vDiffuse;"
			""
			"	Output.vPosition.z *= Input.vPosition.w;"
			""
			"	return Output;"
			"};";

	const char *LINE_PIXEL_SHADER = "struct PS_INPUT"
			"{"
			"	float4 vDiffuse			: TEXCOORD0;"
			"	float4 vPosition		: SV_POSITION;"
			"};"
			""
			""
			"float4 PSMain( PS_INPUT Input ) : SV_TARGET"
			"{"
			"	return Input.vDiffuse;"
			"}";


// Number of vertices to have place for in the buffer at start
	const int NUM_START_LINE_VERTICES = 36;

	RLineRenderer::RLineRenderer(void)
	{
		LineBuffer = nullptr;
		LinePipelineState = nullptr;
		LineCB = nullptr;
	}


	RLineRenderer::~RLineRenderer(void)
	{
		REngine::ResourceCache->DeleteResource(LineBuffer);
		REngine::ResourceCache->DeleteResource(LinePipelineState);
		REngine::ResourceCache->DeleteResource(LineCB);
	}

/** Initializes the buffers and states */
	bool RLineRenderer::InitResources()
	{
		LineBuffer = REngine::ResourceCache->CreateResource<RBuffer>();
		LEB_R(LineBuffer->Init(nullptr, sizeof(LineVertex) * NUM_START_LINE_VERTICES, sizeof(LineVertex),
							   B_VERTEXBUFFER, U_DYNAMIC, CA_WRITE, "LineBuffer"));

		LineCB = REngine::ResourceCache->CreateResource<RBuffer>();
		LEB_R(LineCB->Init(nullptr, sizeof(LineConstantBuffer), sizeof(LineConstantBuffer), B_CONSTANTBUFFER, U_DYNAMIC,
						   CA_WRITE, "LineCB"));

		// Create shaders
		RVertexShader *vs_Lines = RTools::LoadShaderFromString<RVertexShader>(LINE_VERTEX_SHADER, "__VS_Lines");
		RInputLayout *ilLineVx = RTools::CreateInputLayoutFor<LineVertex>(vs_Lines);

		// Simple pixelshader
		RPixelShader *ps_Lines = RTools::LoadShaderFromString<RPixelShader>(LINE_PIXEL_SHADER, "__PS_Lines");

		RStateMachine &sm = REngine::RenderingDevice->GetStateMachine();

		// Create default states
		RDepthStencilState *defaultDSS = nullptr;
		RBlendState *defaultBS = nullptr;
		RSamplerState *defaultSS = nullptr;
		RRasterizerState *defaultRS = nullptr;
		RTools::MakeDefaultStates(&defaultDSS, &defaultSS, &defaultBS, &defaultRS);

		// Create default viewport
		ViewportInfo vpinfo;
		vpinfo.TopLeftX = 0;
		vpinfo.TopLeftY = 0;
		vpinfo.Height = (float) REngine::RenderingDevice->GetOutputResolution().y;
		vpinfo.Width = (float) REngine::RenderingDevice->GetOutputResolution().x;
		vpinfo.MinZ = 0.0f;
		vpinfo.MaxZ = 1.0f;

		// Try to get this from cache
		RViewport *vp = REngine::ResourceCache->GetCachedObject<RViewport>(RTools::HashObject(vpinfo));

		// Create new object if needed
		if (!vp) {
			vp = REngine::ResourceCache->CreateResource<RViewport>();
			vp->CreateViewport(vpinfo);
			REngine::ResourceCache->AddToCache(RTools::HashObject(vpinfo), vp);
		}

		// Create default pipeline state
		sm.SetPrimitiveTopology(EPrimitiveType::PT_LINE_LIST);
		sm.SetBlendState(defaultBS);
		sm.SetRasterizerState(defaultRS);
		sm.SetSamplerState(defaultSS);
		sm.SetDepthStencilState(defaultDSS);
		sm.SetViewport(vp);
		sm.SetConstantBuffer(0, LineCB, EShaderType::ST_VERTEX);
		sm.SetConstantBuffer(0, LineCB, EShaderType::ST_PIXEL);
		sm.SetInputLayout(ilLineVx);
		sm.SetPixelShader(ps_Lines);
		sm.SetVertexShader(vs_Lines);
		sm.SetVertexBuffer(0, LineBuffer);
		sm.SetConstantBuffer(0, LineCB, EShaderType::ST_VERTEX);

		// Save values
		LinePipelineState = sm.MakeDrawCall(0, 0);

		return true;
	}

/** Adds a line to the list */
	bool RLineRenderer::AddLine(const LineVertex &v1, const LineVertex &v2)
	{
		LineCache.push_back(v1);
		LineCache.push_back(v2);

		return true;
	}

/** Flushes the cached lines */
	bool RLineRenderer::Flush(const Matrix &viewProj)
	{
		if (LastFrameFlushed == REngine::RenderingDevice->GetFrameCounter()) {
			LogWarn() << "LineRenderer should only be flushed once per frame!";
			return false;
		}

		if (LineCache.empty())
			return true; // No need to do anything

		LastFrameFlushed = REngine::RenderingDevice->GetFrameCounter();

		// Initialize, if this is our first flush
		if (!LineBuffer)
			LEB_R(InitResources());

		// Get line-data to GPU. Resize buffer automaticall if needed.
		LineBuffer->UpdateData(&LineCache[0], sizeof(LineVertex) * LineCache.size());

		LineConstantBuffer cb;
		cb.M_ViewProj = viewProj;

		// Push constant data to the GPU
		LineCB->UpdateData(&cb);

		// Generate new pipeline-state
		RStateMachine &sm = REngine::RenderingDevice->GetStateMachine();
		sm.SetFromPipelineState(LinePipelineState);

		// Update state
		REngine::ResourceCache->DeleteResource(LinePipelineState);
		LinePipelineState = sm.MakeDrawCall(LineCache.size(), 0);

		// Push to a queue
		RRenderQueueID q = REngine::RenderingDevice->AcquireRenderQueue(false, "Line Queue");
		REngine::RenderingDevice->QueuePipelineState(LinePipelineState, q);

		ClearCache();

		return true;
	}

/** Clears the line cache */
	bool RLineRenderer::ClearCache()
	{
		LineCache.clear();
		return true;
	}

/** Plots a vector of floats */
	void RLineRenderer::PlotNumbers(const std::vector<float> &values, const float3 &location, const float3 &direction,
									float distance, float heightScale, const float4 &color)
	{
		/*for (unsigned int i = 1; i < values.size(); i++) {
			AddLine(LineVertex(location + (direction * (float) (i - 1) * distance) +
							   float3(0, 0, values[i - 1] * heightScale), color),
					LineVertex(location + (direction * (float) i * distance) + float3(0, 0, values[i] * heightScale),
							   color));
		}*/
	}

/** Adds a triangle to the renderlist */
	void RLineRenderer::AddTriangle(const float3 &t0, const float3 &t1, const float3 &t2, const float4 &color)
	{
		AddLine(LineVertex(t0, color), LineVertex(t1, color));
		AddLine(LineVertex(t0, color), LineVertex(t2, color));
		AddLine(LineVertex(t1, color), LineVertex(t2, color));
	}

/** Adds a point locator to the renderlist */
	void RLineRenderer::AddPointLocator(const float3 &location, float size, const float4 &color)
	{
		float3 u = location;
		u.z += size;
		float3 d = location;
		d.z -= size;

		float3 r = location;
		r.x += size;
		float3 l = location;
		l.x -= size;

		float3 b = location;
		b.y += size;
		float3 f = location;
		f.y -= size;

		AddLine(LineVertex(u, color), LineVertex(d, color));
		AddLine(LineVertex(r, color), LineVertex(l, color));
		AddLine(LineVertex(f, color), LineVertex(b, color));
	}

/** Adds a plane to the renderlist */
	void RLineRenderer::AddPlane(const float4 &plane, const float3 &origin, float size, const float4 &color)
	{
		/*float3 pNormal = float3(plane);

		float3 DebugPlaneP1;
		DebugPlaneP1.x = 1;
		DebugPlaneP1.y = 1;
		DebugPlaneP1.z = ((-plane.x - plane.y) / plane.z);

		DebugPlaneP1.Normalize();

		float3 DebugPlaneP2 = pNormal.Cross(DebugPlaneP1);

		//DebugPlaneP2 += SlidingPlaneOrigin;
		float3 &p1 = DebugPlaneP1;
		float3 &p2 = DebugPlaneP2;
		float3 O = origin;

		float3 from;
		float3 to;
		from = (O - p1) - p2;
		to = (O - p1) + p2;
		AddLine(LineVertex(from), LineVertex(to));

		from = (O - p1) + p2;
		to = (O + p1) + p2;
		AddLine(LineVertex(from), LineVertex(to));

		from = (O + p1) + p2;
		to = (O + p1) - p2;
		AddLine(LineVertex(from), LineVertex(to));

		from = (O + p1) - p2;
		to = (O - p1) - p2;
		AddLine(LineVertex(from), LineVertex(to));*/
	}

/** Adds an AABB-Box to the renderlist */
	void RLineRenderer::AddAABB(const float3 &location, float halfSize, const float4 &color)
	{
		// Bottom -x -y -z to +x -y -z
		AddLine(LineVertex(float3(location.x - halfSize, location.y - halfSize, location.z - halfSize), color),
				LineVertex(float3(location.x + halfSize, location.y - halfSize, location.z - halfSize), color));

		AddLine(LineVertex(float3(location.x + halfSize, location.y - halfSize, location.z - halfSize), color),
				LineVertex(float3(location.x + halfSize, location.y + halfSize, location.z - halfSize), color));

		AddLine(LineVertex(float3(location.x + halfSize, location.y + halfSize, location.z - halfSize), color),
				LineVertex(float3(location.x - halfSize, location.y + halfSize, location.z - halfSize), color));

		AddLine(LineVertex(float3(location.x - halfSize, location.y + halfSize, location.z - halfSize), color),
				LineVertex(float3(location.x - halfSize, location.y - halfSize, location.z - halfSize), color));

		// Top
		AddLine(LineVertex(float3(location.x - halfSize, location.y - halfSize, location.z + halfSize), color),
				LineVertex(float3(location.x + halfSize, location.y - halfSize, location.z + halfSize), color));

		AddLine(LineVertex(float3(location.x + halfSize, location.y - halfSize, location.z + halfSize), color),
				LineVertex(float3(location.x + halfSize, location.y + halfSize, location.z + halfSize), color));

		AddLine(LineVertex(float3(location.x + halfSize, location.y + halfSize, location.z + halfSize), color),
				LineVertex(float3(location.x - halfSize, location.y + halfSize, location.z + halfSize), color));

		AddLine(LineVertex(float3(location.x - halfSize, location.y + halfSize, location.z + halfSize), color),
				LineVertex(float3(location.x - halfSize, location.y - halfSize, location.z + halfSize), color));

		// Sides
		AddLine(LineVertex(float3(location.x - halfSize, location.y - halfSize, location.z + halfSize), color),
				LineVertex(float3(location.x - halfSize, location.y - halfSize, location.z - halfSize), color));

		AddLine(LineVertex(float3(location.x + halfSize, location.y - halfSize, location.z + halfSize), color),
				LineVertex(float3(location.x + halfSize, location.y - halfSize, location.z - halfSize), color));

		AddLine(LineVertex(float3(location.x + halfSize, location.y + halfSize, location.z + halfSize), color),
				LineVertex(float3(location.x + halfSize, location.y + halfSize, location.z - halfSize), color));

		AddLine(LineVertex(float3(location.x - halfSize, location.y + halfSize, location.z + halfSize), color),
				LineVertex(float3(location.x - halfSize, location.y + halfSize, location.z - halfSize), color));

	}

/** Adds an AABB-Box to the renderlist */
	void RLineRenderer::AddAABB(const float3 &location, const float3 &halfSize, const float4 &color)
	{
		AddAABBMinMax(float3(location.x - halfSize.x,
							 location.y - halfSize.y,
							 location.z - halfSize.z), float3(location.x + halfSize.x,
															  location.y + halfSize.y,
															  location.z + halfSize.z), color);
	}


	void RLineRenderer::AddAABBMinMax(const float3 &min, const float3 &max, const float4 &color)
	{
		AddLine(LineVertex(float3(min.x, min.y, min.z), color), LineVertex(float3(max.x, min.y, min.z), color));
		AddLine(LineVertex(float3(max.x, min.y, min.z), color), LineVertex(float3(max.x, max.y, min.z), color));
		AddLine(LineVertex(float3(max.x, max.y, min.z), color), LineVertex(float3(min.x, max.y, min.z), color));
		AddLine(LineVertex(float3(min.x, max.y, min.z), color), LineVertex(float3(min.x, min.y, min.z), color));

		AddLine(LineVertex(float3(min.x, min.y, max.z), color), LineVertex(float3(max.x, min.y, max.z), color));
		AddLine(LineVertex(float3(max.x, min.y, max.z), color), LineVertex(float3(max.x, max.y, max.z), color));
		AddLine(LineVertex(float3(max.x, max.y, max.z), color), LineVertex(float3(min.x, max.y, max.z), color));
		AddLine(LineVertex(float3(min.x, max.y, max.z), color), LineVertex(float3(min.x, min.y, max.z), color));

		AddLine(LineVertex(float3(min.x, min.y, min.z), color), LineVertex(float3(min.x, min.y, max.z), color));
		AddLine(LineVertex(float3(max.x, min.y, min.z), color), LineVertex(float3(max.x, min.y, max.z), color));
		AddLine(LineVertex(float3(max.x, max.y, min.z), color), LineVertex(float3(max.x, max.y, max.z), color));
		AddLine(LineVertex(float3(min.x, max.y, min.z), color), LineVertex(float3(min.x, max.y, max.z), color));
	}

/** Adds a ring to the renderlist */
	void RLineRenderer::AddRingZ(const float3 &location, float size, const float4 &color, int res)
	{
		std::vector<float3> points;
		float step = (float) (R_PI * 2) / (float) res;

		for (int i = 0; i < res; i++) {
			points.push_back(
					float3(size * sinf(step * i) + location.x, size * cosf(step * i) + location.y, location.z));
		}

		for (unsigned int i = 0; i < points.size() - 1; i++) {
			AddLine(LineVertex(points[i], color), LineVertex(points[i + 1], color));
		}

		AddLine(LineVertex(points[points.size() - 1], color), LineVertex(points[0], color));
	}
}