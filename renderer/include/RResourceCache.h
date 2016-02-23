#pragma once
#include "RResource.h"

namespace RAPI
{

	class RResource;

	class RResourceCache
	{
	public:
		RResourceCache(void);

		~RResourceCache(void);

		/**
		 * Returns a new resource from the given type.
		 * IMPORTANT: T must be a RResource-Type!
		 */
		template<typename T>
		T *CreateResource()
		{
			// Find the cache-slot of this
			RCache &cache = RCacheTyped<T>::Cache;

			RegisteredCaches.insert(&RCacheTyped<T>::Cache);

			// Check if we got any free-objects
			if (!cache.FreeMemory.empty()) {
				// Get a free index
				unsigned int f = cache.FreeMemory.back();
				cache.FreeMemory.pop_back();

				// Reallocate (Call constructor on memory)
				// TODO: Allocate linearly
				cache.Objects[f] = (RResource *) new(cache.Objects[f])T();

				// Re-set the id
				cache.Objects[f]->SetID(f);

				return (T *) cache.Objects[f];
			} else {
				// Create a totally new object
				T *obj = (T *) new T();
				cache.Objects.push_back(obj);

				// Set the current id
				cache.Objects.back()->SetID((unsigned int)cache.Objects.size() - 1);

				return (T *) obj;
			}
		}

		/**
		 * Unregisters the object, deleting it's members, but keeping the memory
		 * IMPORTANT: T must be a RResource-Type!
		 */
		template<typename T>
		void DeleteResource(T *r)
		{
			if (!r)
				return;

			RResource *resource = (RResource *) r;
			// Find the cache-slot of this
			RCache &cache = RCacheTyped<T>::Cache;

			// Remove from cachelist
			unsigned int id = resource->GetID();

			if (cache.Objects.empty())
				return; // Cache was already deleted in this case. Can happen at the end of the program.

			// Destruct, but keep memory around
			cache.Objects[id]->~RResource();

			// Add the id to the free list
			cache.FreeMemory.push_back(id);
		}

		/**
		 * Returns the object of the given type with the given ID. Nullptr if not found.
		 */
		template<typename T>
		T *GetFromID(unsigned int id)
		{
			RCache &cache = RCacheTyped<T>::Cache;

			if (id >= cache.Objects.size())
				return nullptr;

			return (T *) cache.Objects[id];
		}

		/**
		 * Adds an objects hash to the hashcache, so an object with the same values doesn't
		 * need to be created, but can be reused.
		 */
		template<typename T>
		void AddToCache(size_t hash, T *object)
		{
			RegisteredCaches.insert(&RCacheTyped<T>::Cache);
			RCacheTyped<T>::Cache.HashCache[hash] = object;
		}

		template<typename T>
		void AddToCache(const std::string &alias, T *object)
		{
			RegisteredCaches.insert(&RCacheTyped<T>::Cache);
			RCacheTyped<T>::Cache.HashCache[std::hash<std::string>()(alias)] = object;
		}

		/**
		 * Returns the map-object corresponding to the given class
		 */
		template<typename T>
		const std::unordered_map<size_t, void *> &GetCacheMap()
		{
			return RCacheTyped<T>::Cache.HashCache;
		}

		/**
		 * Returns the object matching with the given hash of the specified type
		 */
		template<typename T>
		T *GetCachedObject(size_t hash)
		{
			auto &map = RCacheTyped<T>::Cache.HashCache;
			auto it = map.find(hash);
			return it != map.end() ? (T *) (*it).second : nullptr;
		}

		template<typename T>
		T *GetCachedObject(const std::string &alias)
		{
			auto &map = RCacheTyped<T>::Cache.HashCache;
			auto it = map.find(std::hash<std::string>()(alias));
			return it != map.end() ? (T *) (*it).second : nullptr;
		}

		/**
		 * Removes an object from the given type and hash from the cache
		 */
		template<typename T>
		void RemoveFromCache(size_t hash)
		{
			RCacheTyped<T>::Cache.HashCache.erase(hash);
		}

		template<typename T>
		void RemoveFromCache(const std::string &alias)
		{
			RCacheTyped<T>::Cache.HashCache.erase(std::hash<std::string>()(alias));
		}

	private:

		struct RCache
		{
			// Currently allocated objects. Not all entities may be valid!
			std::vector<RResource *> Objects;

			// Indices of currently unused and uninitialized objects to be reused
			std::vector<unsigned int> FreeMemory;

			std::unordered_map<size_t, void *> HashCache;
		};

		// Resource caches. The idea is that every templated version of this class has its own
		// memory locations for the static parameters. This way we can statically get the right
		// RCache just by giving the type at compile time
		template<typename T>
		struct RCacheTyped
		{
			static RCache Cache;
		};

		std::set<RCache *> RegisteredCaches;
	};

// Definition of the Cache-Member.
	template<typename T> RResourceCache::RCache RResourceCache::RCacheTyped<T>::Cache;

}