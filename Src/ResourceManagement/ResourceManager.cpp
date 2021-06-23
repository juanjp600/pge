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
    for (auto it = resources.rbegin(); it != resources.rend(); ++it) {
        delete *it;
    }
}

void ResourceManager::increaseSize(size_t count) {
#ifdef DEBUG
    size += count;
#endif
    resources.reserve(resources.capacity() + count);
}
