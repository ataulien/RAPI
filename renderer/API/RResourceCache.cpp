#include "pch.h"
#include "RResourceCache.h"

namespace RAPI
{
	RResourceCache::RResourceCache(void)
	{
	}


	RResourceCache::~RResourceCache(void)
	{
		for (auto it = RegisteredCaches.begin(); it != RegisteredCaches.end(); it++) {
			std::vector<RResource *> &v = (*it)->Objects;

			for(auto& e : v)
				delete e;
		}
	}

}