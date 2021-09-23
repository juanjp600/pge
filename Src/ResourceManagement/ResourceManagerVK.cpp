#include "ResourceManagerVK.h"

#include "../Graphics/GraphicsVK.h"

using namespace PGE;

ResourceManagerVK::ResourceManagerVK(Graphics& gfx) : graphics((GraphicsVK&)gfx) { }

ResourceManagerVK::~ResourceManagerVK() {
    graphics.getDevice().waitIdle();
}

void ResourceManagerVK::trashInternal(const std::list<ResourceBase*>::iterator& it) {
    graphics.trash(**it);
    resources.erase(it);
}
