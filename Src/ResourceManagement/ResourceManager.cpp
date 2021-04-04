#include <ResourceManagement/ResourceManager.h>
#include <ResourceManagement/Resource.h>

#include <cassert>
#include <cstring>

using namespace PGE;

ResourceManager::ResourceManager(int sz) {
    resources = new ResourceBase*[sz];
    nextIndex = 0;
    size = sz;
}

ResourceManager::~ResourceManager() {
    for (int i = nextIndex - 1; i >= 0; i--) {
        delete resources[i];
    }
    delete[] resources;
}

void ResourceManager::addResource(ResourceBase* resource) {
    assert(nextIndex < size);
    resources[nextIndex] = resource;
    nextIndex++;
}

void ResourceManager::increaseSize(int count) {
    size += count;
    ResourceBase** newArr = new ResourceBase*[size];
    memcpy(newArr, resources, nextIndex * sizeof(ResourceBase*));
    delete[] resources;
    resources = newArr;
}
