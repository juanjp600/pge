#include <PGE/StructuredData/StructuredData.h>

using namespace PGE;

StructuredData::ElemLayout::Entry::Entry(const String& nm, int sz) {
    name = nm; size = sz;
}

StructuredData::ElemLayout::LocationAndSize::LocationAndSize(int loc, int sz) {
    location = loc; size = sz;
}

StructuredData::ElemLayout::ElemLayout() {
    elementSize = 0;
}

StructuredData::ElemLayout::ElemLayout(const std::vector<Entry>& entrs) {
    int currLocation = 0;
    for (int i = 0; i < entrs.size(); i++) {
        entries.emplace(entrs[i].name, LocationAndSize(currLocation, entrs[i].size));
        currLocation += entrs[i].size;
    }
    elementSize = currLocation;
}

const StructuredData::ElemLayout::LocationAndSize& StructuredData::ElemLayout::getLocationAndSize(const String& name) const {
    auto iter = entries.find(String::Key(name));
    PGE_ASSERT(iter != entries.end(), "No entry with name \"" + name + "\"");
    return iter->second;
}

int StructuredData::ElemLayout::getElementSize() const {
    return elementSize;
}

bool StructuredData::ElemLayout::operator==(const StructuredData::ElemLayout& other) const {
    if (elementSize != other.elementSize) { return false; }
    if (entries.size() != other.entries.size()) { return false; }
    for (const auto& kvp : entries) {
        const auto& otherKvp = other.entries.find(kvp.first);
        if (otherKvp == other.entries.end()) { return false; }
        if (otherKvp->second.location != kvp.second.location) { return false; }
        if (otherKvp->second.size != kvp.second.size) { return false; }
    }
    return true;
}

StructuredData::StructuredData(const ElemLayout& ly, int elemCount) {
    layout = ly;
    data.resize((size_t)layout.getElementSize() * (size_t)elemCount);
    data.shrink_to_fit();
}

const byte* StructuredData::getData() const {
    return data.data();
}

void StructuredData::setValue(int elemIndex, const String& entryName, float f) {
    memcpy(data.data() + getDataIndex(elemIndex, entryName, sizeof(float)), &f, sizeof(float));
}

void StructuredData::setValue(int elemIndex, const String& entryName, u32 u) {
    memcpy(data.data() + getDataIndex(elemIndex, entryName, sizeof(u32)), &u, sizeof(u32));
}

void StructuredData::setValue(int elemIndex, const String& entryName, const Vector2f& v2f) {
    int dataIndex = getDataIndex(elemIndex, entryName, sizeof(float) * 2);
    memcpy(data.data() + dataIndex, &v2f.x, sizeof(float));
    memcpy(data.data() + dataIndex + sizeof(float), &v2f.y, sizeof(float));
}

void StructuredData::setValue(int elemIndex, const String& entryName, const Vector3f& v3f) {
    int dataIndex = getDataIndex(elemIndex, entryName, sizeof(float) * 3);
    memcpy(data.data() + dataIndex, &v3f.x, sizeof(float));
    memcpy(data.data() + dataIndex + sizeof(float), &v3f.y, sizeof(float));
    memcpy(data.data() + dataIndex + (sizeof(float) * 2), &v3f.z, sizeof(float));
}

void StructuredData::setValue(int elemIndex, const String& entryName, const Vector4f& v4f) {
    int dataIndex = getDataIndex(elemIndex, entryName, sizeof(float) * 4);
    memcpy(data.data() + dataIndex, &v4f.x, sizeof(float));
    memcpy(data.data() + dataIndex + sizeof(float), &v4f.y, sizeof(float));
    memcpy(data.data() + dataIndex + (sizeof(float) * 2), &v4f.z, sizeof(float));
    memcpy(data.data() + dataIndex + (sizeof(float) * 3), &v4f.w, sizeof(float));
}

void StructuredData::setValue(int elemIndex, const String& entryName, const Matrix4x4f& m) {
    memcpy(data.data() + getDataIndex(elemIndex, entryName, sizeof(float) * 4 * 4), m.elements, sizeof(float) * 4 * 4);
}

void StructuredData::setValue(int elemIndex, const String& entryName, const Color& c) {
    int dataIndex = getDataIndex(elemIndex, entryName, sizeof(float) * 4);
    memcpy(data.data() + dataIndex, &c.red, sizeof(float));
    memcpy(data.data() + dataIndex + sizeof(float), &c.green, sizeof(float));
    memcpy(data.data() + dataIndex + (sizeof(float) * 2), &c.blue, sizeof(float));
    memcpy(data.data() + dataIndex + (sizeof(float) * 3), &c.alpha, sizeof(float));
}

int StructuredData::getDataIndex(int elemIndex, const String& entryName, int expectedSize) const {
    PGE_ASSERT(elemIndex >= 0, "Requested a negative element index (" + String::fromInt(elemIndex) + ")");

    int elemOffset = elemIndex * layout.getElementSize();
    PGE_ASSERT(elemOffset <= (data.size() - layout.getElementSize()),
        "Requested an element index greater than the number of elements (" + String::fromInt(elemOffset) + " > " + String::fromInt(data.size() - layout.getElementSize()) + ")");

    const ElemLayout::LocationAndSize& locAndSize = layout.getLocationAndSize(entryName);
    PGE_ASSERT(locAndSize.size == expectedSize,
        "Entry \"" + entryName + "\" size mismatch (expected " + String::fromInt(locAndSize.size) + ", got " + String::fromInt(expectedSize) + ")");

    return elemOffset + locAndSize.location;
}

