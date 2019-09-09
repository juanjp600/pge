#include <Mesh/Mesh.h>

using namespace PGE;

const Vertex::Property Vertex::Property::def = Vertex::Property();

Vertex::Vertex() { }

Vertex::Vertex(const Vertex& other) {
    for (int i=0;i<other.properties.size();i++) {
        properties.push_back(Property(other.properties[i]));
    }
}

Vertex& Vertex::operator=(const Vertex& other) {
    for (int i = 0; i < other.properties.size(); i++) {
        bool found = false;
        for (int j = 0; j < properties.size(); j++) {
            if (properties[j].name.equals(other.properties[i].name)) {
                properties[j].copyOtherValue(other.properties[i]);
                found = true;
            }
        }
        if (found) { continue; }
        properties.push_back(Property(other.properties[i]));
    }
    return *this;
}

Vertex::Property::Value::Value() {
    vector4fVal = Vector4f::zero;
}

Vertex::Property::Property() {
    name = "";
    index = 0;
    type = PROPERTY_TYPE::VECTOR2F;
    value.vector2fVal = Vector2f::zero;
}

Vertex::Property::Property(const Vertex::Property& other) {
    name = other.name;
    copyOtherValue(other);
}

void Vertex::Property::copyOtherValue(const Vertex::Property& other) {
    index = other.index;
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

const Vertex::Property& Vertex::getProperty(const String& name,int indexHint) {
    if (properties[indexHint].name.equals(name)) {
        return properties[indexHint];
    }
    for (int i=0;i<properties.size();i++) {
        if (properties[i].name.getHashCode()>name.getHashCode()) { break; }
        if (properties[i].name.equals(name)) {
            return properties[i];
        }
    }
    return Property::def;
}

void Vertex::setFloat(const String& name,float val) {
    int insertPos = 0;
    for (int i=0;i<properties.size();i++) {
        if (properties[i].name.getHashCode()>name.getHashCode()) { break; }
        if (properties[i].name.equals(name)) {
            properties[i].value.floatVal = val;
            properties[i].type = PROPERTY_TYPE::FLOAT;
            return;
        }
        insertPos = i+1;
    }
    Property prop;
    prop.name = name;
    prop.value.floatVal = val;
    prop.type = PROPERTY_TYPE::FLOAT;
    prop.index = insertPos;
    properties.insert(properties.begin()+insertPos,prop);
}

void Vertex::setUInt(const String& name,unsigned int val) {
    int insertPos = 0;
    for (int i=0;i<properties.size();i++) {
        if (properties[i].name.getHashCode()>name.getHashCode()) { break; }
        if (properties[i].name.equals(name)) {
            properties[i].value.uintVal = val;
            properties[i].type = PROPERTY_TYPE::UINT;
            return;
        }
        insertPos = i+1;
    }
    Property prop;
    prop.name = name;
    prop.value.uintVal = val;
    prop.type = PROPERTY_TYPE::UINT;
    prop.index = insertPos;
    properties.insert(properties.begin()+insertPos,prop);
}

void Vertex::setVector2f(const String& name,Vector2f val) {
    int insertPos = 0;
    for (int i=0;i<properties.size();i++) {
        if (properties[i].name.getHashCode()>name.getHashCode()) { break; }
        if (properties[i].name.equals(name)) {
            properties[i].value.vector2fVal = val;
            properties[i].type = PROPERTY_TYPE::VECTOR2F;
            return;
        }
        insertPos = i+1;
    }
    Property prop;
    prop.name = name;
    prop.value.vector2fVal = val;
    prop.type = PROPERTY_TYPE::VECTOR2F;
    prop.index = insertPos;
    properties.insert(properties.begin()+insertPos,prop);
}

void Vertex::setVector3f(const String& name,Vector3f val) {
    int insertPos = 0;
    for (int i=0;i<properties.size();i++) {
        if (properties[i].name.getHashCode()>name.getHashCode()) { break; }
        if (properties[i].name.equals(name)) {
            properties[i].value.vector3fVal = val;
            properties[i].type = PROPERTY_TYPE::VECTOR3F;
            return;
        }
        insertPos = i+1;
    }
    Property prop;
    prop.name = name;
    prop.value.vector3fVal = val;
    prop.type = PROPERTY_TYPE::VECTOR3F;
    prop.index = insertPos;
    properties.insert(properties.begin()+insertPos,prop);
}

void Vertex::setVector4f(const String& name,Vector4f val) {
    int insertPos = 0;
    for (int i=0;i<properties.size();i++) {
        if (properties[i].name.getHashCode()>name.getHashCode()) { break; }
        if (properties[i].name.equals(name)) {
            properties[i].value.vector4fVal = val;
            properties[i].type = PROPERTY_TYPE::VECTOR4F;
            return;
        }
        insertPos = i+1;
    }
    Property prop;
    prop.name = name;
    prop.value.vector4fVal = val;
    prop.type = PROPERTY_TYPE::VECTOR4F;
    prop.index = insertPos;
    properties.insert(properties.begin()+insertPos,prop);
}

void Vertex::setColor(const String& name,Color val) {
    int insertPos = 0;
    for (int i=0;i<properties.size();i++) {
        if (properties[i].name.getHashCode()>name.getHashCode()) { break; }
        if (properties[i].name.equals(name)) {
            properties[i].value.colorVal = val;
            properties[i].type = PROPERTY_TYPE::COLOR;
            return;
        }
        insertPos = i+1;
    }
    Property prop;
    prop.name = name;
    prop.value.colorVal = val;
    prop.type = PROPERTY_TYPE::COLOR;
    prop.index = insertPos;
    properties.insert(properties.begin()+insertPos,prop);
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
        if (i >= vertices.size()) {
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
