#include <PGE/Graphics/Mesh.h>
#include <PGE/Graphics/Shader.h>

using namespace PGE;

#define PGE_ASSERT_MATERIAL_LAYOUT() PGE_ASSERT(!material.isValid() || verts.getDataSize() <= 0 || material.getShader().getVertexLayout() == verts.getLayout(), "Material must be set before geometry can be set")

void Mesh::setGeometry(const StructuredData& verts, const std::vector<Line>& lines) {
    setGeometry(verts.copy(), lines);
}

void Mesh::setGeometry(const StructuredData& verts, const std::vector<Triangle>& triangles) {
    setGeometry(verts.copy(), triangles);
}

void Mesh::setGeometry(StructuredData&& verts, const std::vector<Line>& lines) {
    PGE_ASSERT_MATERIAL_LAYOUT();

    vertices = std::move(verts);
    indices.clear();
    indices.reserve(lines.size() * 2);
    for (int i = 0; i < lines.size(); i++) {
        indices.emplace_back(lines[i].indices[0]);
        indices.emplace_back(lines[i].indices[1]);
    }
    primitiveType = PrimitiveType::LINE;

    mustReuploadInternalData = true;
}

void Mesh::setGeometry(StructuredData&& verts, const std::vector<Triangle>& triangles) {
    PGE_ASSERT_MATERIAL_LAYOUT();

    vertices = std::move(verts);
    indices.clear();
    indices.reserve(triangles.size() * 3);
    for (int i = 0; i < triangles.size(); i++) {
        indices.emplace_back(triangles[i].indices[0]);
        indices.emplace_back(triangles[i].indices[1]);
        indices.emplace_back(triangles[i].indices[2]);
    }
    primitiveType = PrimitiveType::TRIANGLE;

    mustReuploadInternalData = true;
}

void Mesh::setGeometry(StructuredData&& verts, PrimitiveType type, std::vector<u32>&& inds) {
    PGE_ASSERT_MATERIAL_LAYOUT();
    PGE_ASSERT(type == PrimitiveType::LINE && inds.size() % 2 == 0 || type == PrimitiveType::TRIANGLE && inds.size() % 3 == 0,
            "Invalid primitive type or inadequate indices count");

    vertices = std::move(verts);
    indices = std::move(inds);
    primitiveType = type;
    mustReuploadInternalData = true;
}

void Mesh::clearGeometry() {
    indices.clear();
    primitiveType.reset();
    mustReuploadInternalData = true;
}

void Mesh::setMaterial(const Material& m) {
    PGE_ASSERT(
        !m.isValid() ||
        vertices.getDataSize() <= 0 ||
        m.getShader().getVertexLayout() == vertices.getLayout(),
        "Can't set material with mismatched vertex layout without discarding"
    );
    material = m;
}

bool Mesh::isOpaque() const {
    return material.isOpaque();
}

void Mesh::render() {
    if (primitiveType.has_value() && material.isValid()) {
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

Mesh::Material::Material() {
    shader = nullptr;
    opaque = Opaque::YES;
}

Mesh::Material::Material(Shader& sh, Opaque o) {
    shader = &sh;
    opaque = o;
}

Mesh::Material::Material(Shader& sh, Texture& tex, Opaque o) {
    shader = &sh;
    textures = { tex };
    opaque = o;
}

Mesh::Material::Material(Shader& sh, const ReferenceVector<Texture>& texs, Opaque o) {
    shader = &sh;
    textures = texs;
    opaque = o;
}

bool Mesh::Material::isValid() const {
    return shader != nullptr;
}

bool Mesh::Material::isOpaque() const {
    return opaque == Opaque::YES;
}

Shader& Mesh::Material::getShader() const {
    PGE_ASSERT(isValid(), "Material is not valid!");
    return *shader;
}

size_t Mesh::Material::getTextureCount() const {
    PGE_ASSERT(isValid(), "Material is not valid!");
    return textures.size();
}

Texture& Mesh::Material::getTexture(size_t index) const {
    PGE_ASSERT(isValid(), "Material is not valid!");
    return textures.at(index);
}
