#include <PGE/ResourceManagement/ResourceManager.h>
#include <PGE/Exception/Exception.h>

using namespace PGE;

ResourceManager::ResourceManager(size_t sz) {
    resources.reserve(sz);
    size = sz;
}

ResourceManager::~ResourceManager() {
    for (auto it = resources.rbegin(); it != resources.rend(); ++it) {
        delete *it;
    }
}

void ResourceManager::increaseSize(size_t count) {
    size += count;
    resources.reserve(std::max(size, resources.capacity()));
}
