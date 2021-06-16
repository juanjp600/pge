#include <PGE/ResourceManagement/ResourceManager.h>

#ifdef DEBUG
#include <PGE/Exception/Exception.h>
#endif

using namespace PGE;

ResourceManager::ResourceManager(size_t sz) {
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

void ResourceManager::increaseSize(int count) {
#ifdef DEBUG
    size += count;
#endif
    resources.reserve(resources.capacity() + count);
}
