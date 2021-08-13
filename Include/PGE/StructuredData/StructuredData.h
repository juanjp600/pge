#ifndef PGE_MEMORYLAYOUT_H_INCLUDED
#define PGE_MEMORYLAYOUT_H_INCLUDED

#include <PGE/Types/Types.h>
#include <PGE/String/String.h>
#include <PGE/String/Key.h>
#include <PGE/Math/Vector.h>
#include <PGE/Math/Matrix.h>
#include <PGE/Color/Color.h>
#include <PGE/ResourceManagement/PolymorphicHeap.h>

#include <unordered_map>

namespace PGE {

class StructuredData : NoHeap {
    public:
        class ElemLayout {
            public:
                struct Entry : private NoHeap {
                    Entry(const String& nm, int sz);

                    String name;
                    int size;
                };

                struct LocationAndSize : private NoHeap {
                    LocationAndSize(int loc, int sz);

                    // TODO: C++20 default.
                    bool operator==(const LocationAndSize& other) const;

                    int location;
                    int size;
                };

                ElemLayout() = default;
                ElemLayout(const std::vector<Entry>& entrs);

                const LocationAndSize& getLocationAndSize(const String& name) const;
                const LocationAndSize& getLocationAndSize(const String::Key& name) const;
                int getElementSize() const;

                bool operator==(const StructuredData::ElemLayout& other) const;
            private:
                std::unordered_map<String::Key, LocationAndSize> entries;
                int elementSize;
        };

        StructuredData() = default;
        StructuredData(const ElemLayout& ly, int elemCount);

        StructuredData(StructuredData&& other) noexcept;
        void operator=(StructuredData&& other) noexcept;

        StructuredData copy() const;

        const byte* getData() const;
        size_t getDataSize() const;
        const ElemLayout& getLayout() const;

        template <typename T>
        void setValue(int elemIndex, const String& entryName, const T& value) {
            setValue(elemIndex, String::Key(entryName), value);
        }
        void setValue(int elemIndex, const String::Key& entry, float f);
        void setValue(int elemIndex, const String::Key& entry, u32 u);
        void setValue(int elemIndex, const String::Key& entry, const Vector2f& v2f);
        void setValue(int elemIndex, const String::Key& entry, const Vector3f& v3f);
        void setValue(int elemIndex, const String::Key& entry, const Vector4f& v4f);
        void setValue(int elemIndex, const String::Key& entry, const Matrix4x4f& m);
        void setValue(int elemIndex, const String::Key& entry, const Color& c);

    private:
        int getDataIndex(int elemIndex, const String::Key& entry, int expectedSize) const;

        ElemLayout layout; //don't change this to a pointer, stop preemptively optimizing!!!!!
        std::unique_ptr<byte[]> data;
        size_t size;
};

}

#endif // PGE_MEMORYLAYOUT_H_INCLUDED
