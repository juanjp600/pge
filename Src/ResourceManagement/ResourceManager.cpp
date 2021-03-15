#include <ResourceManagement/ResourceManager.h>
#include <ResourceManagement/ResourceOwner.h>

using namespace PGE;

ResourceManager::ResourceManager() { }
ResourceManager::~ResourceManager() {
    for (int i = (int)resourceOwners.size() - 1; i >= 0; i--) {
        delete resourceOwners[i];
    }
    resourceOwners.clear();
}

void ResourceManager::addResource(ResourceOwnerBase* resourceOwner) {
    resourceOwners.push_back(resourceOwner);
}
