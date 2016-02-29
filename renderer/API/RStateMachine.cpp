#include "pch.h"
#include "RStateMachine.h"
#include "RPipelineState.h"
#include "RResourceCache.h"
#include "REngine.h"
#include "RDevice.h"
#include "RPipelineState.h"
#include "RTexture.h"
#include "RRasterizerState.h"
#include "RBlendState.h"
#include "RSamplerState.h"
#include "RDepthStencilState.h"
#include "RBuffer.h"
#include "RPixelShader.h"
#include "RVertexShader.h"
#include "RInputLayout.h"
#include "RViewport.h"
#include "RTools.h"

namespace RAPI
{

	RStateMachine::RStateMachine(void)
	{
		memset(&Changes, 0, sizeof(Changes));
		memset(&ChangesCount, 0, sizeof(ChangesCount));

		Invalidate();

		// This can actually crash the nvidia driver if not set correctly
		State.BoundIDs.PrimitiveType = EPrimitiveType::PT_TRIANGLE_LIST;
	}


	RStateMachine::~RStateMachine(void)
	{
	}

/**
 * Collects all resources from the small state and sets them to the current full state
 */
	void RStateMachine::SetFromPipelineState(const RPipelineState *state)
	{
		RResourceCache *cache = REngine::ResourceCache;

		if (state->IDs.PrimitiveType != State.BoundIDs.PrimitiveType) {
			Changes.PrimitiveType = true;
			ChangesCount.PrimitiveType++;
		}

		if (state->IDs.BlendState != State.BoundIDs.BlendState) {
			State.BlendState = cache->GetFromID<RBlendState>(state->IDs.BlendState);
			Changes.BlendState = true;
			ChangesCount.BlendState++;
		}

		if (state->IDs.RasterizerState != State.BoundIDs.RasterizerState) {
			State.RasterizerState = cache->GetFromID<RRasterizerState>(state->IDs.RasterizerState);
			Changes.RasterizerState = true;
			ChangesCount.RasterizerState++;
		}

		if (state->IDs.SamplerState != State.BoundIDs.SamplerState) {
			State.SamplerState = cache->GetFromID<RSamplerState>(state->IDs.BlendState);
			Changes.SamplerState = true;
			ChangesCount.SamplerState++;
		}
		if (state->IDs.DepthStencilState != State.BoundIDs.DepthStencilState) {
			State.DepthStencilState = cache->GetFromID<RDepthStencilState>(state->IDs.BlendState);
			Changes.DepthStencilState = true;
			ChangesCount.DepthStencilState++;
		}
		if (state->IDs.IndexBuffer != State.BoundIDs.IndexBuffer) {
			State.IndexBuffer = cache->GetFromID<RBuffer>(state->IDs.IndexBuffer);
			Changes.IndexBuffer = true;
			ChangesCount.IndexBuffer++;
		}
		if (state->IDs.VertexBuffer0 != State.BoundIDs.VertexBuffer0) {
			State.VertexBuffers[0] = cache->GetFromID<RBuffer>(state->IDs.VertexBuffer0);
			Changes.VertexBuffers[0] = true;
			ChangesCount.VertexBuffers[0]++;
		}
		if (state->IDs.VertexBuffer1 != State.BoundIDs.VertexBuffer1) {
			State.VertexBuffers[1] = cache->GetFromID<RBuffer>(state->IDs.VertexBuffer1);
			Changes.VertexBuffers[1] = true;
			ChangesCount.VertexBuffers[1]++;
		}

		if (state->IDs.VertexShader != State.BoundIDs.VertexShader) {
			State.VertexShader = cache->GetFromID<RVertexShader>(state->IDs.VertexShader);
			Changes.VertexShader = true;
			ChangesCount.VertexShader++;
		}

		if (state->IDs.PixelShader != State.BoundIDs.PixelShader) {
			State.PixelShader = cache->GetFromID<RPixelShader>(state->IDs.PixelShader);
			Changes.PixelShader = true;
			ChangesCount.PixelShader++;
		}

		if (state->IDs.InputLayout != State.BoundIDs.InputLayout) {
			State.InputLayout = cache->GetFromID<RInputLayout>(state->IDs.InputLayout);
			Changes.InputLayout = true;
			ChangesCount.InputLayout++;
		}

		if (state->IDs.ViewportID != State.BoundIDs.ViewportID) {
			State.Viewport = cache->GetFromID<RViewport>(state->IDs.ViewportID);
			Changes.Viewport = true;
			ChangesCount.Viewport++;
		}

		for (int i = 0; i < EShaderType::ST_NUM_SHADER_TYPES; i++) {
			if (state->_NumConstantBuffers[i] && state->_ConstantBuffersHash != State._ConstantBuffersHash) {
				memcpy(&State.ConstantBuffers[i], &state->ConstantBuffers[i], state->_NumConstantBuffers[i] * sizeof(state->ConstantBuffers[i]));	
				Changes.ConstantBuffers[i] = true;
				ChangesCount.ConstantBuffers[i]++;
			}
			if (state->_NumStructuredBuffers[i] && state->_StructuredBuffersHash != State._StructuredBuffersHash) {
				memcpy(&State.StructuredBuffers[i], &state->StructuredBuffers[i], state->_NumStructuredBuffers[i] * sizeof(state->StructuredBuffers[i]));	
				Changes.StructuredBuffers[i] = true;
				ChangesCount.StructuredBuffers[i]++;
			}

			if (state->_NumTextures[i] && state->_TexturesHash != State._TexturesHash) {
				memcpy(&State.Textures[i], &state->Textures[i], state->_NumTextures[i] * sizeof(state->Textures[i]));	
				State.Textures[i] = state->Textures[i];
				Changes.MainTexture = true;
				ChangesCount.MainTexture++;
			}
		}

		State.NumDrawElements = state->NumDrawElements;
		State.StartIndexOffset = state->StartIndexOffset;
		State.StartVertexOffset = state->StartVertexOffset;
		State.StartInstanceOffset = state->StartInstanceOffset;
		State.BoundIDs = state->IDs;
	}

	void RStateMachine::SetFromPipelineState(const struct RPipelineState *state, const ChangesStruct &changes)
	{
		RResourceCache *cache = REngine::ResourceCache;

		if (changes.BlendState) {
			State.BlendState = cache->GetFromID<RBlendState>(state->IDs.BlendState);
		}

		if (changes.RasterizerState) {
			State.RasterizerState = cache->GetFromID<RRasterizerState>(state->IDs.RasterizerState);
		}

		if (changes.SamplerState) {
			State.SamplerState = cache->GetFromID<RSamplerState>(state->IDs.SamplerState);
		}
		if (changes.DepthStencilState) {
			State.DepthStencilState = cache->GetFromID<RDepthStencilState>(state->IDs.DepthStencilState);
		}
		if (changes.IndexBuffer) {
			State.IndexBuffer = cache->GetFromID<RBuffer>(state->IDs.IndexBuffer);
		}
		if (changes.VertexBuffers[0]) {
			State.VertexBuffers[0] = cache->GetFromID<RBuffer>(state->IDs.VertexBuffer0);
		}
		if (changes.VertexBuffers[1]) {
			State.VertexBuffers[1] = cache->GetFromID<RBuffer>(state->IDs.VertexBuffer1);
		}

		if (changes.VertexShader) {
			State.VertexShader = cache->GetFromID<RVertexShader>(state->IDs.VertexShader);
		}

		if (changes.PixelShader) {
			State.PixelShader = cache->GetFromID<RPixelShader>(state->IDs.PixelShader);
		}

		if (changes.InputLayout) {
			State.InputLayout = cache->GetFromID<RInputLayout>(state->IDs.InputLayout);
		}

		if (changes.Viewport) {
			State.Viewport = cache->GetFromID<RViewport>(state->IDs.ViewportID);
		}

		for (int i = 0; i < EShaderType::ST_NUM_SHADER_TYPES; i++) {
			if (changes.ConstantBuffers[i]) {
				memcpy(&State.ConstantBuffers[i], &state->ConstantBuffers[i], state->_NumConstantBuffers[i] * sizeof(state->ConstantBuffers[i]));				
			}

			if (changes.StructuredBuffers[i]) {
				memcpy(&State.StructuredBuffers[i], &state->StructuredBuffers[i], state->_NumStructuredBuffers[i] * sizeof(state->StructuredBuffers[i]));	
			}

			if (changes.MainTexture) {
				memcpy(&State.Textures[i], &state->Textures[i], state->_NumTextures[i] * sizeof(state->Textures[i]));	
			}
		}

		State.NumDrawElements = state->NumDrawElements;
		State.StartIndexOffset = state->StartIndexOffset;
		State.StartVertexOffset = state->StartVertexOffset;
		State.StartInstanceOffset = state->StartInstanceOffset;
		State.BoundIDs = state->IDs;
	}

/**
 * Sets the values from a pipeline-state-object to the current state
 */
	void RStateMachine::AssignPipelineStateValues(RPipelineState *state)
	{
		// Set draw-order now that everything else is set up
		SetDrawOrderFor(&State);

		for (int i = 0; i < EShaderType::ST_NUM_SHADER_TYPES; i++) {
			state->ConstantBuffers[i] = State.ConstantBuffers[i];
			state->_NumConstantBuffers[i] = 0;

			// Count resources
			for(int j=0;j<RAPI_MAX_NUM_SHADER_RESOURCES;j++)
				if(state->ConstantBuffers[i][j])
					state->_NumConstantBuffers[i] = j + 1;

			// Generate hash		
			state->_ConstantBuffersHash[i] = RTools::HashObject(state->ConstantBuffers[i]);
		}

		for (int i = 0; i < EShaderType::ST_NUM_SHADER_TYPES; i++) {
			state->Textures[i] = State.Textures[i];
			state->_NumTextures[i] = 0;

			// Count resources
			for(int j=0;j<RAPI_MAX_NUM_SHADER_RESOURCES;j++)
				if(state->Textures[i][j])
					state->_NumTextures[i] = j + 1;

			// Generate hash		
			state->_TexturesHash[i] = RTools::HashObject(state->Textures[i]);
		}

		for (int i = 0; i < EShaderType::ST_NUM_SHADER_TYPES; i++) {
			state->StructuredBuffers[i] = State.StructuredBuffers[i];
			state->_NumStructuredBuffers[i] = 0;

			// Count resources
			for(int j=0;j<RAPI_MAX_NUM_SHADER_RESOURCES;j++)
				if(state->StructuredBuffers[i][j])
					state->_NumStructuredBuffers[i] = j + 1;

			// Generate hash		
			state->_StructuredBuffersHash[i] = RTools::HashObject(state->StructuredBuffers[i]);
			
		}

		state->IDs = State.BoundIDs;


	}

/**
* Figures out where the state belongs to in the current draw order
*/
	void RStateMachine::SetDrawOrderFor(RPipelineStateFull *state)
	{
		// Figure out draw-order
		RPipelineState::EDrawOrder d = RPipelineState::DO_OPAQUE;

		// Check for blending
		if (State.BlendState && State.BlendState->GetStateInfo().BlendEnabled) {
			// Check for type
			switch (State.BlendState->GetStateInfo().BlendTypeHint) {
				case RBlendStateInfo::BTH_ADDITIVE:
					d = RPipelineState::DO_ALPHA_BLEND;
					break;

				case RBlendStateInfo::BTH_BLEND:
					d = RPipelineState::DO_ALPHA_BLEND;
					break;

				case RBlendStateInfo::BTH_MASKED:
					d = RPipelineState::DO_ALPHA_TEST;
					break;

				default:
					break;
			}
		}

		state->BoundIDs.DrawOrder = d;
	}

/**
 * Invalidates the current state. Useful for when a new frame starts for example
 */
	void RStateMachine::Invalidate()
	{
		// Force rebind of all states in all cases
		memset(&Changes, 1, sizeof(Changes));
		memset(&State.BoundIDs, 0xFF, sizeof(State.BoundIDs));

		for (int i = 0; i < EShaderType::ST_NUM_SHADER_TYPES; i++) {
			State.Textures[i].fill(nullptr);
			State.ConstantBuffers[i].fill(nullptr);
			State.StructuredBuffers[i].fill(nullptr);
		}
	}

/**
* Sets all changes back to false
*/
	void RStateMachine::ResetChanges()
	{
		// Set all to false
		memset(&Changes, 0, sizeof(Changes));
	}

	RPipelineState *RStateMachine::MakeDrawCall(unsigned int numVertices, unsigned int startVertexOffset)
	{
		RPipelineState *s = REngine::ResourceCache->CreateResource<RPipelineState>();
		AssignPipelineStateValues(s);

		s->NumDrawElements = numVertices;
		s->StartVertexOffset = startVertexOffset;
		s->IDs.DrawFunctionID = EDrawCallType::DCT_Draw;

		return s;
	}

	RPipelineState *RStateMachine::MakeDrawCallIndexed(unsigned int numIndices, unsigned int startIndexOffset,
													   unsigned int startVertexOffset)
	{
		RPipelineState *s = REngine::ResourceCache->CreateResource<RPipelineState>();
		AssignPipelineStateValues(s);

		s->NumDrawElements = numIndices;
		s->StartVertexOffset = startVertexOffset;
		s->StartIndexOffset = startIndexOffset;
		s->IDs.DrawFunctionID = EDrawCallType::DCT_DrawIndexed;

		return s;
	}

	RPipelineState *RStateMachine::MakeDrawCallIndexedInstanced(unsigned int numIndices,
																unsigned int numInstances,
																unsigned int startIndexOffset,
																unsigned int startVertexOffset,
																unsigned int startInstanceOffset)
	{
		RPipelineState *s = REngine::ResourceCache->CreateResource<RPipelineState>();
		AssignPipelineStateValues(s);

		s->NumDrawElements = numIndices;
		s->NumInstances = numInstances;
		s->StartVertexOffset = startVertexOffset;
		s->StartIndexOffset = startIndexOffset;
		s->StartInstanceOffset = startInstanceOffset;
		s->IDs.DrawFunctionID = EDrawCallType::DCT_DrawIndexedInstanced;

		return s;
	}

	void RStateMachine::SetPixelShader(RPixelShader *shader)
	{
		State.PixelShader = shader;
		State.BoundIDs.PixelShader = shader->GetID();
	}

	void RStateMachine::SetVertexShader(RVertexShader *shader)
	{
		State.VertexShader = shader;
		State.BoundIDs.VertexShader = shader->GetID();
	}

	void RStateMachine::SetInputLayout(RInputLayout *layout)
	{
		State.InputLayout = layout;

		State.BoundIDs.InputLayout = layout ? layout->GetID() : -1;
	}

	void RStateMachine::SetRasterizerState(RRasterizerState *state)
	{
		State.RasterizerState = state;
		State.BoundIDs.RasterizerState = state->GetID();
	}

	void RStateMachine::SetSamplerState(RSamplerState *state)
	{
		State.SamplerState = state;
		State.BoundIDs.SamplerState = state->GetID();
	}

	void RStateMachine::SetBlendState(RBlendState *state)
	{
		State.BlendState = state;
		State.BoundIDs.BlendState = state->GetID();
	}

	void RStateMachine::SetDepthStencilState(RDepthStencilState *state)
	{
		State.DepthStencilState = state;
		State.BoundIDs.DepthStencilState = state->GetID();
	}

	void RStateMachine::SetPrimitiveTopology(EPrimitiveType type)
	{
		State.BoundIDs.PrimitiveType = type;
	}

	void RStateMachine::SetTexture(unsigned int slot, RTexture *texture, EShaderType stage)
	{
		if (State.Textures[stage].size() <= slot)
			return;

		State.Textures[stage][slot] = texture;

		if (!texture)
			State.BoundIDs.MainTexture = -1;
		else if (stage == EShaderType::ST_PIXEL && slot == 0)
			State.BoundIDs.MainTexture = texture->GetID();
	}

	void RStateMachine::SetConstantBuffer(unsigned int slot, RBuffer *buffer, EShaderType stage)
	{
		if (State.Textures[stage].size() <= slot)
			return;

		State.ConstantBuffers[stage][slot] = buffer;
	}

	void RStateMachine::SetVertexBuffer(unsigned int slot, RBuffer *buffer)
	{
		State.VertexBuffers[slot] = buffer;

		if (buffer) {
			if (slot == 0)
				State.BoundIDs.VertexBuffer0 = buffer->GetID();
			else
				State.BoundIDs.VertexBuffer1 = buffer->GetID();
		}
		else {
			if (slot == 0)
				State.BoundIDs.VertexBuffer0 = -1;
			else
				State.BoundIDs.VertexBuffer1 = -1;
		}
	}

	void RStateMachine::SetIndexBuffer(RBuffer *buffer)
	{
		State.IndexBuffer = buffer;
		State.BoundIDs.IndexBuffer = buffer ? buffer->GetID() : -1;;
	}

	void RStateMachine::SetViewport(RViewport *viewport)
	{
		State.Viewport = viewport;
		State.BoundIDs.ViewportID = viewport->GetID();
	}

	void RStateMachine::SetStructuredBuffer(unsigned int slot, RBuffer *buffer, EShaderType stage)
	{
		if (State.Textures[stage].size() <= slot)
			return;

		State.StructuredBuffers[stage][slot] = buffer;
	}
}