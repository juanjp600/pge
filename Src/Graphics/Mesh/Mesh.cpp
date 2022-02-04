#include <PGE/Graphics/Mesh.h>
#include <PGE/Graphics/Shader.h>
#include <PGE/Graphics/Material.h>

using namespace PGE;

void Mesh::setGeometry(StructuredData&& verts, PrimitiveType type, std::vector<u32>&& inds) {
    assertMaterialLayout(verts);
    using enum PrimitiveType;
    PGE_ASSERT(type == LINE && inds.size() % 2 == 0 || type == TRIANGLE && inds.size() % 3 == 0,
            "Invalid primitive type or inadequate indices count");

    vertices = std::move(verts);
    indices = std::move(inds);
    primitiveType = type;
    mustReuploadInternalData = true;
}

void Mesh::clearGeometry() {
    vertices = StructuredData();
    indices.clear();
    primitiveType.reset();
    mustReuploadInternalData = true;
}

void Mesh::setMaterial(Material* m) {
    PGE_ASSERT(
        m == nullptr ||
        vertices.getDataSize() <= 0 ||
        m->getShader().getVertexLayout() == vertices.getLayout(),
        "Can't set material with mismatched vertex layout without discarding"
    );
    material = m;
}

// TODO: Implement for OGL and DX11.
void Mesh::setUpdateStrategy(UpdateStrategy us) {
    strategy = us;
}

bool Mesh::isOpaque() const {
    return material->isOpaque();
}

void Mesh::render() {
    if (primitiveType.has_value() && material != nullptr) {
        if (mustReuploadInternalData) {
            uploadInternalData();
            mustReuploadInternalData = false;
        }
        renderInternal();
    }
}

Mesh::Line::Line(u32 a, u32 b) {
    indices[0] = a; indices[1] = b;
}

Mesh::Triangle::Triangle(u32 a, u32 b, u32 c) {
    indices[0] = a; indices[1] = b; indices[2] = c;
}

void Mesh::assertMaterialLayout(const StructuredData& verts, const std::source_location& location) {
    PGE_ASSERT_AT(material == nullptr
        || verts.getDataSize() <= 0
        || material->getShader().getVertexLayout() == verts.getLayout(),
        "Material must be set before geometry can be set", location);
}
