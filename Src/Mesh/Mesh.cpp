#include <Mesh/Mesh.h>

using namespace PGE;

const Vertex::Property Vertex::Property::def = Property();

Vertex::Property::Value::Value() {
    vector4fVal = Vector4f::zero;
}

const Vertex::Property& Vertex::getProperty(const String& name) {
    for (int i=0;i<properties.size();i++) {
        if (properties[i].name.equals(name)) {
            return properties[i];
        }
    }
    return Property::def;
}

void Vertex::setFloat(const String& name,float val) {
    for (int i=0;i<properties.size();i++) {
        if (properties[i].name.equals(name)) {
            properties[i].value.floatVal = val;
            properties[i].type = PROPERTY_TYPE::FLOAT;
            return;
        }
    }
    Property prop;
    prop.name = name;
    prop.value.floatVal = val;
    prop.type = PROPERTY_TYPE::FLOAT;
    properties.push_back(prop);
}

void Vertex::setUInt(const String& name,unsigned int val) {
    for (int i=0;i<properties.size();i++) {
        if (properties[i].name.equals(name)) {
            properties[i].value.uintVal = val;
            properties[i].type = PROPERTY_TYPE::UINT;
            return;
        }
    }
    Property prop;
    prop.name = name;
    prop.value.uintVal = val;
    prop.type = PROPERTY_TYPE::UINT;
    properties.push_back(prop);
}

void Vertex::setVector2f(const String& name,Vector2f val) {
    for (int i=0;i<properties.size();i++) {
        if (properties[i].name.equals(name)) {
            properties[i].value.vector2fVal = val;
            properties[i].type = PROPERTY_TYPE::VECTOR2F;
            return;
        }
    }
    Property prop;
    prop.name = name;
    prop.value.vector2fVal = val;
    prop.type = PROPERTY_TYPE::VECTOR2F;
    properties.push_back(prop);
}

void Vertex::setVector3f(const String& name,Vector3f val) {
    for (int i=0;i<properties.size();i++) {
        if (properties[i].name.equals(name)) {
            properties[i].value.vector3fVal = val;
            properties[i].type = PROPERTY_TYPE::VECTOR3F;
            return;
        }
    }
    Property prop;
    prop.name = name;
    prop.value.vector3fVal = val;
    prop.type = PROPERTY_TYPE::VECTOR3F;
    properties.push_back(prop);
}

void Vertex::setVector4f(const String& name,Vector4f val) {
    for (int i=0;i<properties.size();i++) {
        if (properties[i].name.equals(name)) {
            properties[i].value.vector4fVal = val;
            properties[i].type = PROPERTY_TYPE::VECTOR4F;
            return;
        }
    }
    Property prop;
    prop.name = name;
    prop.value.vector4fVal = val;
    prop.type = PROPERTY_TYPE::VECTOR4F;
    properties.push_back(prop);
}

void Vertex::setColor(const String& name,Color val) {
    for (int i=0;i<properties.size();i++) {
        if (properties[i].name.equals(name)) {
            properties[i].value.colorVal = val;
            properties[i].type = PROPERTY_TYPE::COLOR;
            return;
        }
    }
    Property prop;
    prop.name = name;
    prop.value.colorVal = val;
    prop.type = PROPERTY_TYPE::COLOR;
    properties.push_back(prop);
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
    isDirty = true;
    vertices = verts; //TODO: check for property mismatches?
    primitives = prims;
    opaque = true; //TODO: check for colors
}

void Mesh::clearGeometry() {
    isDirty = true;
    vertices.clear();
    primitives.clear();
    opaque = true;
}

void Mesh::setMaterial(Material* m) {
    isDirty = true;
    material = m;
    opaque = m->isOpaque();
    //TODO: check for vertex colors
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
