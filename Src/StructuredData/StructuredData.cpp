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
    PGE_ASSERT(iter != entries.end(), "No entry with key \"" + String::hexFromInt(key.hash) + "\"");
    return iter->second;
}

int StructuredData::ElemLayout::getElementSize() const {
    return elementSize;
}

StructuredData::StructuredData(const ElemLayout& ly, int elemCount) {
    layout = ly;
    size = (size_t)layout.getElementSize() * elemCount;
    data = std::make_unique<byte[]>(size);
}

StructuredData::StructuredData(StructuredData&& other) noexcept {
    layout = other.layout;
    size = other.size;
    data = std::move(other.data);
}

void StructuredData::operator=(StructuredData&& other) noexcept {
    layout = other.layout;
    size = other.size;
    data = std::move(other.data);
}

StructuredData StructuredData::copy() const {
    StructuredData ret;
    ret.layout = layout;
    ret.size = size;
    if (size > 0) {
        ret.data = std::make_unique<byte[]>(size);
        memcpy(ret.data.get(), data.get(), size);
    }
    return ret;
}

const byte* StructuredData::getData() const {
    return data.get();
}

int StructuredData::getDataSize() const {
    return size;
}

int StructuredData::getElementCount() const {
    if (size <= 0 || layout.getElementSize() <= 0) { return 0; }
    return size / layout.getElementSize();
}

const StructuredData::ElemLayout& StructuredData::getLayout() const {
    return layout;
}

int StructuredData::getDataIndex(int elemIndex, const String::Key& entry, int expectedSize) const {
    PGE_ASSERT(elemIndex >= 0, "Requested a negative element index (" + String::from(elemIndex) + ")");

    int elemOffset = elemIndex * layout.getElementSize();
    PGE_ASSERT(elemOffset <= (size - layout.getElementSize()),
        "Requested an element index greater than the number of elements ("
        + String::from(elemOffset) + " > " + String::from((int)(size - layout.getElementSize())) + ")");

    const ElemLayout::LocationAndSize& locAndSize = layout.getLocationAndSize(entry);
    PGE_ASSERT(locAndSize.size == expectedSize,
        "Entry \"" + String::hexFromInt(entry.hash) + "\" size mismatch (expected " + String::from(locAndSize.size)
        + ", got " + String::from(expectedSize) + ")");

    return elemOffset + locAndSize.location;
}

