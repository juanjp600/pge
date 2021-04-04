#include <ResourceManagement/ResourceManager.h>

#ifdef DEBUG
#include <cassert>
#endif

using namespace PGE;

ResourceManager::ResourceManager(int sz) {
    resources.reserve(sz);
#ifdef DEBUG
    size = sz;
#endif
}

ResourceManager::~ResourceManager() {
    for (int i = resources.size() - 1; i >= 0; i--) {
        delete resources[i];
    }
}

void ResourceManager::addResource(ResourceBase* resource) {
#ifdef DEBUG
    assert(size > resources.size());
#endif
    resources.push_back(resource);
}

void ResourceManager::increaseSize(int count) {
#ifdef DEBUG
    size += count;
#endif
    resources.reserve(resources.capacity() + count);
}
