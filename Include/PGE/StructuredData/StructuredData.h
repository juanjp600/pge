#ifndef PGE_MEMORYLAYOUT_H_INCLUDED
#define PGE_MEMORYLAYOUT_H_INCLUDED

#include <PGE/Types/Types.h>
#include <PGE/String/String.h>
#include <PGE/String/Key.h>
#include <PGE/Math/Vector.h>
#include <PGE/Math/Matrix.h>
#include <PGE/Color/Color.h>
#include <PGE/Types/PolymorphicHeap.h>
#include <PGE/Types/Concepts.h>

#include <unordered_map>

namespace PGE {

template <typename T>
concept StructuredType = OneOf<T, float, u32, Vector2f, Vector3f, Vector4f, Matrix4x4f, Color>;

class StructuredData {
    public:
        class ElemLayout {
            public:
                struct Entry {
                    Entry(const String& nm, int sz);

                    String name;
                    int size;
                };

                struct LocationAndSize {
                    LocationAndSize(int loc, int sz);

                    bool operator==(const LocationAndSize& other) const = default;

                    int location;
                    int size;
                };

                ElemLayout() = default;
                ElemLayout(const Enumerable<Entry> auto& entrs) {
                    int currLocation = 0;
                    for (const auto& entry : entrs) {
                        entries.emplace(entry.name, LocationAndSize(currLocation, entry.size));
                        currLocation += entry.size;
                    }
                    elementSize = currLocation;
                }

                const LocationAndSize& getLocationAndSize(const String& name) const;
                const LocationAndSize& getLocationAndSize(const String::Key& name) const;
                int getElementSize() const;

                bool operator==(const StructuredData::ElemLayout& other) const = default;
            private:
                int elementSize;
                std::unordered_map<String::Key, LocationAndSize> entries;
        };

        StructuredData() = default;
        StructuredData(const ElemLayout& ly, int elemCount);

        StructuredData(const StructuredData&) = delete;
        void operator=(const StructuredData&) = delete;

        StructuredData(StructuredData&& other) noexcept;
        void operator=(StructuredData&& other) noexcept;

        StructuredData copy() const;

        const byte* getData() const;
        int getDataSize() const;
        int getElementCount() const;
        const ElemLayout& getLayout() const;

        void setValue(int elemIndex, const String& entry, const StructuredType auto& value) {
            setValue(elemIndex, String::Key(entry), value);
        }

        void setValue(int elemIndex, const String::Key& entry, const StructuredType auto& value) {
            memcpy(data.get() + getDataIndex(elemIndex, entry, sizeof(value)), &value, sizeof(value));
        }

    private:
        int getDataIndex(int elemIndex, const String::Key& entry, int expectedSize) const;

        ElemLayout layout; //don't change this to a pointer, stop preemptively optimizing!!!!!
        std::unique_ptr<byte[]> data;
        int size;
};

}

#endif // PGE_MEMORYLAYOUT_H_INCLUDED
