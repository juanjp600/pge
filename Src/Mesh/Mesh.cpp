#include <Mesh/Mesh.h>

#include <Graphics/Graphics.h>
#include <Exception/Exception.h>

using namespace PGE;

Vertex::Property::Property(const Vertex::Property& other) {
    type = other.type;
    switch (type) {
        case Type::VECTOR2F: {
            value.vector2fVal = other.value.vector2fVal;
        } break;
        case Type::VECTOR3F: {
            value.vector3fVal = other.value.vector3fVal;
        } break;
        case Type::VECTOR4F: {
            value.vector4fVal = other.value.vector4fVal;
        } break;
        case Type::COLOR: {
            value.colorVal = other.value.colorVal;
        } break;
        case Type::FLOAT: {
            value.floatVal = other.value.floatVal;
        } break;
        case Type::UINT: {
            value.uintVal = other.value.uintVal;
        } break;
        default: {
            PGE_ASSERT(false, "Invalid vertex property type!");
        }
    }
}

const Vertex::Property& Vertex::getProperty(const String& name) const {
    return properties.find(name)->second;
}

void Vertex::setFloat(const String& name, float val) {
    Property& prop = properties[name];
    prop.value.floatVal = val;
    prop.type = Property::Type::FLOAT;
}

void Vertex::setUInt(const String& name, unsigned int val) {
    Property& prop = properties[name];
    prop.value.uintVal = val;
    prop.type = Property::Type::UINT;
}

void Vertex::setVector2f(const String& name, const Vector2f& val) {
    Property& prop = properties[name];
    prop.value.vector2fVal = val;
    prop.type = Property::Type::VECTOR2F;
}

void Vertex::setVector3f(const String& name, const Vector3f& val) {
    Property& prop = properties[name];
    prop.value.vector3fVal = val;
    prop.type = Property::Type::VECTOR3F;
}

void Vertex::setVector4f(const String& name, const Vector4f& val) {
    Property& prop = properties[name];
    prop.value.vector4fVal = val;
    prop.type = Property::Type::VECTOR4F;
}

void Vertex::setColor(const String& name, const Color& val) {
    Property& prop = properties[name];
    prop.value.colorVal = val;
    prop.type = Property::Type::COLOR;
}

Primitive::Primitive(long ia,long ib) {
    a = ia; b = ib; c = -1;
}

Primitive::Primitive(long ia,long ib,long ic) {
    a = ia; b = ib; c = ic;
}

Mesh* Mesh::clone() {
    Mesh* newMesh = create(graphics, primitiveType);
    newMesh->setGeometry(vertexCount, vertices, primitiveCount, primitives);
    newMesh->setMaterial(material);
    return newMesh;
}

void Mesh::setGeometry(int vertCount, const std::vector<Vertex>& verts, int primCount, const std::vector<Primitive>& prims) {
    mustUpdateInternalData = true; mustReuploadInternalData = true;
    //TODO: check for property mismatches?
    for (int i = 0; i < vertCount; i++) {
        if (i >= (int)vertices.size()) {
            vertices.push_back(verts[i]);
        } else {
            vertices[i] = verts[i];
        }
    }
    vertexCount = vertCount;

    //TODO: optimize this too?
    primitiveCount = primCount;
    primitives = prims;
    opaque = material!=nullptr ? material->isOpaque() : true;
}

void Mesh::setGeometry(const std::vector<Vertex>& verts, const std::vector<Primitive>& prims) {
    setGeometry((int)verts.size(), verts, (int)prims.size(), prims);
}

void Mesh::clearGeometry() {
    mustUpdateInternalData = true; mustReuploadInternalData = true;
    vertexCount = 0; primitiveCount = 0;
    opaque = material!=nullptr ? material->isOpaque() : true;
}

void Mesh::setMaterial(Material* m) {
    //mustUpdateInternalData = true; mustReuploadInternalData = true;
    material = m;
    opaque = material != nullptr ? material->isOpaque() : true;
}

const std::vector<Vertex>& Mesh::getVertices(int& vertCount) const {
    vertCount = vertexCount;
    return vertices;
}

const std::vector<Primitive>& Mesh::getPrimitives(int& primCount) const {
    primCount = primitiveCount;
    return primitives;
}

bool Mesh::isOpaque() const {
    return opaque;
}
