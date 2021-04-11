#include <Mesh/Mesh.h>

#include <Graphics/Graphics.h>
#include <Exception/Exception.h>

using namespace PGE;

const Vertex::Property Vertex::Property::def = Vertex::Property();

Vertex::Vertex() { }

Vertex::Vertex(const Vertex& other) {
    for (int i = 0; i < (int)other.properties.size(); i++) {
        properties.push_back(Property(other.properties[i]));
    }
}

Vertex& Vertex::operator=(const Vertex& other) {
    for (int i = 0; i < (int)other.properties.size(); i++) {
        bool found = false;
        insertProperty(other.properties[i].hashCode);
        for (int j = 0; j < (int)properties.size(); j++) {
            if (properties[j].hashCode == other.properties[i].hashCode) {
                properties[j].copyOtherValue(other.properties[i]);
                found = true;
                break;
            }
        }
        __ASSERT(found, "Failed to copy vertex property (hashcode: " + String::format(other.properties[i].hashCode, "%llX") + ")");
    }
    return *this;
}

Vertex::Property::Value::Value() {
    vector4fVal = Vector4f::zero;
}

Vertex::Property::Property() {
    hashCode = 0;
    type = PROPERTY_TYPE::VECTOR2F;
    value.vector2fVal = Vector2f::zero;
}

Vertex::Property::Property(const Vertex::Property& other) {
    hashCode = other.hashCode;
    copyOtherValue(other);
}

void Vertex::Property::copyOtherValue(const Vertex::Property& other) {
    type = other.type;
    switch (type) {
        case PROPERTY_TYPE::VECTOR2F: {
            value.vector2fVal = other.value.vector2fVal;
        } break;
        case PROPERTY_TYPE::VECTOR3F: {
            value.vector3fVal = other.value.vector3fVal;
        } break;
        case PROPERTY_TYPE::VECTOR4F: {
            value.vector4fVal = other.value.vector4fVal;
        } break;
        case PROPERTY_TYPE::COLOR: {
            value.colorVal = other.value.colorVal;
        } break;
        case PROPERTY_TYPE::FLOAT: {
            value.floatVal = other.value.floatVal;
        } break;
        case PROPERTY_TYPE::UINT: {
            value.uintVal = other.value.uintVal;
        } break;
    }
}

const Vertex::Property& Vertex::getProperty(const String& name,int& indexHint) {
    if ((indexHint < (int)properties.size()) && properties[indexHint].hashCode == name.getHashCode()) {
        return properties[indexHint];
    }
    for (int i = 0; i < (int)properties.size(); i++) {
        if (properties[i].hashCode>name.getHashCode()) { break; }
        if (properties[i].hashCode == name.getHashCode()) {
            indexHint = i;
            return properties[i];
        }
    }
    return Property::def;
}

Vertex::Property& Vertex::insertProperty(long long hashCode) {
    int insertPos = 0;
    for (int i = 0; i < (int)properties.size(); i++) {
        if (properties[i].hashCode>hashCode) { break; }
        if (properties[i].hashCode == hashCode) {
            return properties[i];
        }
        insertPos = i+1;
    }
    Property prop;
    prop.hashCode = hashCode;
    properties.insert(properties.begin()+insertPos,prop);

    for (int i = 0; i < (int)properties.size()-1; i++) {
        __ASSERT(properties[i].hashCode <= properties[i + 1].hashCode, "Incorrect vertex property order (" + String::fromInt(i) + ", " + String::fromInt(i + 1) + ")");
    }

    return properties[insertPos];
}

void Vertex::setFloat(const String& name,float val) {
    Property& prop = insertProperty(name.getHashCode());
    prop.value.floatVal = val;
    prop.type = PROPERTY_TYPE::FLOAT;
}

void Vertex::setUInt(const String& name,unsigned int val) {
    Property& prop = insertProperty(name.getHashCode());
    prop.value.uintVal = val;
    prop.type = PROPERTY_TYPE::UINT;
}

void Vertex::setVector2f(const String& name,Vector2f val) {
    Property& prop = insertProperty(name.getHashCode());
    prop.value.vector2fVal = val;
    prop.type = PROPERTY_TYPE::VECTOR2F;
}

void Vertex::setVector3f(const String& name,Vector3f val) {
    Property& prop = insertProperty(name.getHashCode());
    prop.value.vector3fVal = val;
    prop.type = PROPERTY_TYPE::VECTOR3F;
}

void Vertex::setVector4f(const String& name,Vector4f val) {
    Property& prop = insertProperty(name.getHashCode());
    prop.value.vector4fVal = val;
    prop.type = PROPERTY_TYPE::VECTOR4F;
}

void Vertex::setColor(const String& name,Color val) {
    Property& prop = insertProperty(name.getHashCode());
    prop.value.colorVal = val;
    prop.type = PROPERTY_TYPE::COLOR;
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
    setGeometry(verts.size(), verts, prims.size(), prims);
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
