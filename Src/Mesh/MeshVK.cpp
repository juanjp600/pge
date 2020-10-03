#include "MeshVK.h"

#include <Graphics/Graphics.h>

using namespace PGE;

#ifdef VULKAN
Mesh* Mesh::create(Graphics* gfx, Primitive::TYPE pt) {
    return new MeshVK(gfx, pt);
}
#endif

MeshVK::MeshVK(Graphics* gfx, Primitive::TYPE pt) {

}

MeshVK::~MeshVK() {

}

void MeshVK::updateInternalData() {

}

void MeshVK::render() {

}

void MeshVK::cleanup() {

}

void MeshVK::throwException(String func, String details) {

}

void MeshVK::uploadInternalData() {

}
