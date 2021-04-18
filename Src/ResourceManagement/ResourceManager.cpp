#include <ResourceManagement/ResourceManager.h>

#ifdef DEBUG
#include <Exception/Exception.h>
#endif

using namespace PGE;

ResourceManager::ResourceManager(int sz) {
    resources.reserve(sz);
#ifdef DEBUG
    size = sz;
#endif
}

ResourceManager::~ResourceManager() {
    for (int i = (int)resources.size() - 1; i >= 0; i--) {
        delete resources[i];
    }
}

void ResourceManager::increaseSize(int count) {
#ifdef DEBUG
    size += count;
#endif
    resources.reserve(resources.capacity() + count);
}
