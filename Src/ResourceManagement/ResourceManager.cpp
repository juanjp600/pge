#include <PGE/ResourceManagement/ResourceManager.h>
#include <PGE/Exception/Exception.h>

using namespace PGE;

ResourceManager::ResourceManager() { }

ResourceManager::~ResourceManager() {
    for (auto it = resources.rbegin(); it != resources.rend(); ++it) {
        delete *it;
    }
}
