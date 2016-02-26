#include "pch.h"
#include "RTools.h"
#include "RVertexShader.h"
#include "REngine.h"
#include "RTools.h"
#include <functional>
#include "RDepthStencilState.h"
#include "RBlendState.h"
#include "RSamplerState.h"
#include "RRasterizerState.h"

using namespace RAPI;
using namespace RTools;



/**
 * Creates default instances of the state-objects and caches them using the 'default'-alias
 */
void RTools::MakeDefaultStates(RDepthStencilState **defDSS, RSamplerState **defSS, RBlendState **defBS,
                       RRasterizerState **defRS)
{
    // Create the default versions of the states
    if(defDSS) REngine::ResourceCache->AddToCache("default", GetState(RDepthStencilStateInfo().SetDefault()));
	if(defSS) REngine::ResourceCache->AddToCache("default", GetState(RSamplerStateInfo().SetDefault()));
	if(defBS) REngine::ResourceCache->AddToCache("default", GetState(RBlendStateInfo().SetDefault()));
	if(defRS) REngine::ResourceCache->AddToCache("default", GetState(RRasterizerStateInfo().SetDefault()));

    // Create some specific ones
    RRasterizerStateInfo rsinfo = RRasterizerStateInfo().SetDefault();
    rsinfo.CullMode = RRasterizerStateInfo::CM_CULL_NONE;
    REngine::ResourceCache->AddToCache("twosided", GetState(rsinfo));

    // And output
	if(defDSS) *defDSS = GetState(RDepthStencilStateInfo().SetDefault());
	if(defSS) *defSS = GetState(RSamplerStateInfo().SetDefault());
	if(defRS) *defRS = GetState(RRasterizerStateInfo().SetDefault());
	if(defBS) *defBS = GetState(RBlendStateInfo().SetDefault());
}
