#include <Mesh/Mesh.h>

using namespace PGE;

const Vertex::Property Vertex::Property::def = Vertex::Property();

Vertex::Vertex() { }

Vertex::Vertex(const Vertex& other) {
    for (int i=0;i<other.properties.size();i++) {
        properties.push_back(Property(other.properties[i]));
    }
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
    newMesh->setGeometry(vertices,primitives);
    newMesh->setMaterial(material);
    return newMesh;
}

void Mesh::setGeometry(const std::vector<Vertex>& verts,const std::vector<Primitive>& prims) {
    mustUpdateInternalData = true; mustReuploadInternalData = true;
    vertices = verts; //TODO: check for property mismatches?
    primitives = prims;
    opaque = material!=nullptr ? material->isOpaque() : true;
}

void Mesh::clearGeometry() {
    mustUpdateInternalData = true; mustReuploadInternalData = true;
    vertices.clear();
    primitives.clear();
    opaque = material!=nullptr ? material->isOpaque() : true;
}

void Mesh::setMaterial(Material* m) {
    //mustUpdateInternalData = true; mustReuploadInternalData = true;
    material = m;
    opaque = material != nullptr ? material->isOpaque() : true;
}

const std::vector<Vertex>& Mesh::getVertices() const {
    return vertices;
}

const std::vector<Primitive>& Mesh::getPrimitives() const {
    return primitives;
}

bool Mesh::isOpaque() const {
    return opaque;
}
