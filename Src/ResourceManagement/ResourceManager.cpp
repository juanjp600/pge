#include <ResourceManagement/ResourceManager.h>
#include <ResourceManagement/ResourceOwner.h>

#include <cassert>
#include <cstring>

using namespace PGE;

ResourceManager::ResourceManager(int sz) {
    resourceOwners = new ResourceOwnerBase*[sz];
    lastIndex = 0;
    size = sz;
}

ResourceManager::~ResourceManager() {
    for (int i = lastIndex - 1; i >= 0; i--) {
        delete resourceOwners[i];
    }
    delete[] resourceOwners;
}

void ResourceManager::addResource(ResourceOwnerBase* resourceOwner) {
    assert(lastIndex < size);
    resourceOwners[lastIndex] = resourceOwner;
    lastIndex++;
}

void ResourceManager::increaseSize(int count) {
    size += count;
    ResourceOwnerBase** newOwners = new ResourceOwnerBase*[size];
    memcpy(newOwners, resourceOwners, lastIndex * sizeof(ResourceOwnerBase*));
    delete[] resourceOwners;
    resourceOwners = newOwners;
}
