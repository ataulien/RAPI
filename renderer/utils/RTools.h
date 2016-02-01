#pragma once
#include <set>
#include <vector>
#include <list>
#include "REngine.h"
#include "RResourceCache.h"
#include "RInputLayout.h"
#include "RRasterizerState.h"
#include "RBlendState.h"
#include "RDepthStencilState.h"
#include "RSamplerState.h"
#include "RLineRenderer.h"
#include "RTweakBar.h"
#include "RPixelShader.h"
#include "RVertexShader.h"

#ifdef __GNUC__
#include <pthread.h>
#endif

namespace RAPI
{

/**
 * Useful function for handling rendering specific things
 */
	class RVertexShader;

	class RDepthStencilState;

	class RSamplerState;

	class RBlendState;

	class RRasterizerState;

	struct RRasterizerStateInfo;
	struct RDepthStencilStateInfo;
	struct RBlendStateInfo;
	struct RSamplerStateInfo;
	namespace RTools
	{
		/** Line renderer */
		SELECTANY RLineRenderer LineRenderer;
		SELECTANY RTweakBar TweakBar;

		/** Shader loading functions, which also cache the objects by using the alias as hash */
		//static RVertexShader* LoadVertexShader(const std::string& file,
		//	const std::string& alias,
		//	const std::vector<std::vector<std::string>>& definitions);

		/** Returns the ID of the calling thread */
		static uint32_t GetCurrentThreadId()
		{
#ifdef WIN32
            return ::GetCurrentThreadId();
#else
			return pthread_self();
#endif
		}

		/** Erases an element by value from a vector */
		template<typename T> void EraseByElement(std::vector<T>& vector, T value)
		{
			auto it = std::find(vector.begin(), vector.end(), value);

			if(it != vector.end())
				vector.erase(it);
		}

		/** Erases an element by value from a vector */
		template<typename T, typename S> void EraseByElement(std::map<T, S>& map, S value)
		{
			for(auto it = map.begin();it!=map.end();it++)
			{
				if((*it).second == value)
					it = map.erase(it);
			}
		}

		/** Deletes all elements of the given std::vector */
		template<typename T> void DeleteElements(std::vector<T>& vector)
		{
			for(auto it = vector.begin(); it != vector.end(); it++)
			{
				delete (*it);
			}

			vector.clear();
		}

		/** Deletes all elements of the given std::list */
		template<typename T> void DeleteElements(std::list<T>& list)
		{
			for(auto it = list.begin(); it != list.end(); it++)
			{
				delete (*it);
			}

			list.clear();
		}

		/** Deletes all elements of the given std::set */
		template<typename T> void DeleteElements(std::set<T>& set)
		{
			for(auto it = set.begin(); it != set.end(); it++)
			{
				delete (*it);
			}

			set.clear();
		}

		/** Deletes all (second) elements of the given std::map */
		template<typename T, typename S> void DeleteElements(std::map<T, S>& map)
		{
			for(auto it = map.begin(); it != map.end(); it++)
			{
				delete (*it).second;
			}

			map.clear();
		}

		/** Deletes all (second) elements of the given std::unordered_map */
		template<typename T, typename S> void DeleteElements(std::unordered_map<T, S>& map)
		{
			for(auto it = map.begin(); it != map.end(); it++)
			{
				delete (*it).second;
			}

			map.clear();
		}

		static std::size_t hash_value(float value)
		{
			std::hash<float> hasher;
			return hasher(value);
		}

		static std::size_t hash_value(uint32_t value)
		{
			std::hash<uint32_t> hasher;
			return hasher(value);
		}

		static void hash_combine(std::size_t& seed, float value)
		{
			seed ^= hash_value(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}

		/** Hashes the given DWORD value */
		static void hash_combine(std::size_t& seed, uint32_t value)
		{
			seed ^= hash_value(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}

		/** Hashes the data of the given type */
		template<typename T> size_t HashObject(const T& object)
		{
			byte* data = (byte *)&object;
			size_t hash = 0;
			for(int i=0;i<sizeof(T);i+=4)
			{
				RDWORD d;
				((char *)&d)[0] = data[i];
				((char *)&d)[1] = data[i+1];
				((char *)&d)[2] = data[i+2];
				((char *)&d)[3] = data[i+3];

				hash_combine(hash, d);
			}

			return hash;
		}

		/** Reloads all shaders */
		static void ReloadShaders()
		{
			auto &psmap = REngine::ResourceCache->GetCacheMap<RPixelShader>();
			auto &vsmap = REngine::ResourceCache->GetCacheMap<RVertexShader>();

			for (auto s : psmap)
				((RPixelShader *) s.second)->ReloadShader();

			for (auto s : vsmap)
				((RVertexShader *) s.second)->ReloadShader();
		}

		/** Generic state creation function */
		template<typename T, typename S>
		static T *GetState(const S &info)
		{
			size_t hash = HashObject(info);
			T *s = REngine::ResourceCache->GetCachedObject<T>(hash);

			// Create states if missing
			if (!s)
			{
				s = REngine::ResourceCache->CreateResource<T>();
				s->CreateState(info);
				REngine::ResourceCache->AddToCache(hash, s);
			}

			return s;
		}

		/** State-creation wrappers */
		static RRasterizerState *GetState(const RRasterizerStateInfo &info)
		{
			return GetState<RRasterizerState, RRasterizerStateInfo>(info);
		}

		static RDepthStencilState *GetState(const RDepthStencilStateInfo &info)
		{
			return GetState<RDepthStencilState, RDepthStencilStateInfo>(info);
		}

		static RBlendState *GetState(const RBlendStateInfo &info)
		{
			return GetState<RBlendState, RBlendStateInfo>(info);
		}

		static RSamplerState *GetState(const RSamplerStateInfo &info)
		{
			return GetState<RSamplerState, RSamplerStateInfo>(info);
		}


		/** Shader loading functions, which also cache the objects by using the alias as hash */
		template<typename T>
		static T *LoadShader(const std::string &file,
							 const std::string &alias,
							 const std::vector<std::vector<std::string>> &definitions = std::vector<std::vector<std::string>>())
		{
			// Check if this was already loaded
			RResourceCache &cache = *REngine::ResourceCache;
			T *shader = cache.GetCachedObject<T>(alias);

			if (shader)
				return shader;

			// Not in cache, load it
			shader = cache.CreateResource<T>();
			if (!shader->LoadShader(file, definitions))
				return false;

			// Add it to cache
			cache.AddToCache(alias, shader);

			return shader;
		}

		/** Shader loading functions, which also cache the objects by using the alias as hash */
		template<typename T>
		static T *LoadShaderFromString(const std::string &shadercode,
									   const std::string &alias,
									   const std::vector<std::vector<std::string>> &definitions = std::vector<std::vector<std::string>>())
		{
			// Check if this was already loaded
			RResourceCache &cache = *REngine::ResourceCache;
			T *shader = cache.GetCachedObject<T>(alias);

			if (shader)
				return shader;

			// Not in cache, load it
			shader = cache.CreateResource<T>();
			if (!shader->LoadShaderFromString(shadercode, definitions))
				return nullptr;

			// Add it to cache
			cache.AddToCache(alias, shader);

			return shader;
		}

		/** Creates an InputLayout for the given vertextype an decleration. It will be cached as well, so this
            is save to be called multiple times for the same layout.
            The Vertexdesc has to have an INPUT_LAYOUT_DESC static member. */
		template<typename T>
		static RInputLayout *CreateInputLayoutFor(RVertexShader *vsTemplate)
		{
			// Try to fetch one from cache
			RResourceCache &cache = *REngine::ResourceCache;
			RInputLayout *layout = cache.GetCachedObject<RInputLayout>(
					std::hash<void *>()((void *) T::INPUT_LAYOUT_DESC));

			if (layout)
				return layout;

			// Create a new one
			layout = cache.CreateResource<RInputLayout>();
			layout->CreateInputLayout(vsTemplate, T::INPUT_LAYOUT_DESC, ARRAYSIZE(T::INPUT_LAYOUT_DESC));

			cache.AddToCache(std::hash<void *>()((void *) T::INPUT_LAYOUT_DESC), layout);

			return layout;
		}

		/**
         * Creates default instances of the state-objects and caches them using the 'default'-alias
         */
		void MakeDefaultStates(RDepthStencilState **defDSS, RSamplerState **defSS, RBlendState **defBS,
							   RRasterizerState **defRS);

		/** Indexes the given vertex array
         *	T: Vertex-Type
         *	I: Index-Type
         *	C: Comparison-class, defining the ()-operator and
         *			static void FoundDuplicate(T& toStay, const T& toRemove)
         */
		template<typename C, typename T, typename I>
		static void IndexVertices(T *input, unsigned int numInputVertices, std::vector<T> &outVertices,
								  std::vector<I> &outIndices)
		{
			typename std::set<std::pair<T, int>, C> vertices;
			int index = 0;

			for (unsigned int i = 0; i < numInputVertices; i++)
			{
				typename std::set<std::pair<T, int>, C>::iterator it = vertices.find(
						std::make_pair(input[i], 0/*this value doesn't matter*/));
				if (it != vertices.end())
				{
					std::pair<T, int> vxNew = (*it);
					vertices.erase(it);
					C::FoundDuplicate(vxNew.first, input[i]);
					vertices.insert(vxNew);
					outIndices.push_back(vxNew.second);
				}
				else
				{
					/*char* c = (char *)&input[i].color;
                    c[0] = rand() % 255;
                    c[1] = rand() % 255;
                    c[2] = rand() % 255;*/

					vertices.insert(std::make_pair(input[i], index));
					outIndices.push_back(index++);
				}
			}
			/*
            // Check for overlaying triangles and throw them out
            // Some mods do that for the worldmesh for example
            std::set<std::tuple<I,I,I>> triangles;
            for(unsigned int i=0;i<outIndices.size();i+=3)
            {
                // Insert every combination of indices here. Duplicates will be ignored
                triangles.insert(std::make_tuple(outIndices[i+0], outIndices[i+1], outIndices[i+2]));
            }

            // Extract the cleaned triangles to the indices vector
            outIndices.clear();
            for(auto it = triangles.begin(); it != triangles.end(); it++)
            {
                outIndices.push_back(std::get<0>((*it)));
                outIndices.push_back(std::get<1>((*it)));
                outIndices.push_back(std::get<2>((*it)));
            }
            */

			// Notice that the vertices in the set are not sorted by the index
			// so you'll have to rearrange them like this:
			outVertices.clear();
			outVertices.resize(vertices.size());
			for (auto it = vertices.begin(); it != vertices.end(); it++)
			{
				if ((unsigned int) it->second >= vertices.size())
				{
					continue;
				}

				outVertices[it->second] = it->first;
			}
		}

		/** Converts a triangle fan to a list */
		template<typename T>
		static void TriangleFanToList(T *input, unsigned int numInputVertices, std::vector<T> &outVertices)
		{
			for (unsigned int i = 1; i < numInputVertices - 1; i++)
			{
				outVertices.push_back(input[0]);
				outVertices.push_back(input[i + 1]);
				outVertices.push_back(input[i]);
			}
		}

		/**
         * Converts a RDWORD-Color to float4
         */
		static float4 RDWORDToFloat4(RDWORD color)
		{
			byte a = color >> 24;
			byte r = (color >> 16) & 0xFF;
			byte g = (color >> 8) & 0xFF;
			byte b = color & 0xFF;

			return float4(r / 255.0f,
						  g / 255.0f,
						  b / 255.0f,
						  a / 255.0f);
		}

		/**
         * Converts a float4-Color to RDWORD. Values are clamped to 1. Stored as argb, where a is the 4th byte.
         */
		static RDWORD float4ToRDWORD(const float4 &color)
		{
			RDWORD dw;
			byte *v = (byte *) &dw;
			v[0] = std::min((byte) 255, (byte) (color.z * 255.0f));
			v[1] = std::min((byte) 255, (byte) (color.y * 255.0f));
			v[2] = std::min((byte) 255, (byte) (color.x * 255.0f));
			v[3] = std::min((byte) 255, (byte) (color.w * 255.0f));

			return dw;
		}
	};
}