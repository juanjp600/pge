#include <PGE/StructuredData/StructuredData.h>

using namespace PGE;

StructuredData::ElemLayout::Entry::Entry(const String& nm, int sz) {
    name = nm; size = sz;
}

StructuredData::ElemLayout::LocationAndSize::LocationAndSize(int loc, int sz) {
    location = loc; size = sz;
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
    return getLocationAndSize(String::Key(name));
}

const StructuredData::ElemLayout::LocationAndSize& StructuredData::ElemLayout::getLocationAndSize(const String::Key& key) const {
    auto iter = entries.find(key);
    PGE_ASSERT(iter != entries.end(), "No entry with key \"" + String::format(key.hash, "%Xll") + "\"");
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
    layout = &ly;
    size = (size_t)layout->getElementSize() * elemCount;
    data = std::make_unique<byte[]>(size);
}

StructuredData::StructuredData(const StructuredData& other) {
    layout = other.layout;
    size = other.size;
    data = std::make_unique<byte[]>(size);
    memcpy(data.get(), other.data.get(), size);
}

StructuredData& StructuredData::operator=(const StructuredData& other) {
    layout = other.layout;
    size = other.size;
    data = std::make_unique<byte[]>(size);
    memcpy(data.get(), other.data.get(), size);
    return *this;
}

const byte* StructuredData::getData() const {
    return data.get();
}

size_t StructuredData::getDataSize() const {
    return size;
}

const StructuredData::ElemLayout* StructuredData::getLayout() const {
    return layout;
}

void StructuredData::setValue(int elemIndex, const String::Key& entry, float f) {
    memcpy(data.get() + getDataIndex(elemIndex, entry, sizeof(float)), &f, sizeof(float));
}

void StructuredData::setValue(int elemIndex, const String::Key& entry, u32 u) {
    memcpy(data.get() + getDataIndex(elemIndex, entry, sizeof(u32)), &u, sizeof(u32));
}

void StructuredData::setValue(int elemIndex, const String::Key& entry, const Vector2f& v2f) {
    int dataIndex = getDataIndex(elemIndex, entry, sizeof(float) * 2);
    memcpy(data.get() + dataIndex, &v2f.x, sizeof(float));
    memcpy(data.get() + dataIndex + sizeof(float), &v2f.y, sizeof(float));
}

void StructuredData::setValue(int elemIndex, const String::Key& entry, const Vector3f& v3f) {
    int dataIndex = getDataIndex(elemIndex, entry, sizeof(float) * 3);
    memcpy(data.get() + dataIndex, &v3f.x, sizeof(float));
    memcpy(data.get() + dataIndex + sizeof(float), &v3f.y, sizeof(float));
    memcpy(data.get() + dataIndex + (sizeof(float) * 2), &v3f.z, sizeof(float));
}

void StructuredData::setValue(int elemIndex, const String::Key& entry, const Vector4f& v4f) {
    int dataIndex = getDataIndex(elemIndex, entry, sizeof(float) * 4);
    memcpy(data.get() + dataIndex, &v4f.x, sizeof(float));
    memcpy(data.get() + dataIndex + sizeof(float), &v4f.y, sizeof(float));
    memcpy(data.get() + dataIndex + (sizeof(float) * 2), &v4f.z, sizeof(float));
    memcpy(data.get() + dataIndex + (sizeof(float) * 3), &v4f.w, sizeof(float));
}

void StructuredData::setValue(int elemIndex, const String::Key& entry, const Matrix4x4f& m) {
    memcpy(data.get() + getDataIndex(elemIndex, entry, sizeof(float) * 4 * 4), m.elements, sizeof(float) * 4 * 4);
}

void StructuredData::setValue(int elemIndex, const String::Key& entry, const Color& c) {
    int dataIndex = getDataIndex(elemIndex, entry, sizeof(float) * 4);
    memcpy(data.get() + dataIndex, &c.red, sizeof(float));
    memcpy(data.get() + dataIndex + sizeof(float), &c.green, sizeof(float));
    memcpy(data.get() + dataIndex + (sizeof(float) * 2), &c.blue, sizeof(float));
    memcpy(data.get() + dataIndex + (sizeof(float) * 3), &c.alpha, sizeof(float));
}

int StructuredData::getDataIndex(int elemIndex, const String::Key& entry, int expectedSize) const {
    PGE_ASSERT(elemIndex >= 0, "Requested a negative element index (" + String::fromInt(elemIndex) + ")");

    int elemOffset = elemIndex * layout->getElementSize();
    PGE_ASSERT(elemOffset <= (size - layout->getElementSize()),
        "Requested an element index greater than the number of elements ("
        + String::fromInt(elemOffset) + " > " + String::fromInt((int)(size - layout->getElementSize())) + ")");

    const ElemLayout::LocationAndSize& locAndSize = layout->getLocationAndSize(entry);
    PGE_ASSERT(locAndSize.size == expectedSize,
        "Entry \"" + String::format(entry.hash, "%Xll") + "\" size mismatch (expected " + String::fromInt(locAndSize.size)
        + ", got " + String::fromInt(expectedSize) + ")");

    return elemOffset + locAndSize.location;
}

