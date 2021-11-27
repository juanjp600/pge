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

        StructuredData(const StructuredData&) = delete;
        void operator=(const StructuredData&) = delete;

        StructuredData(StructuredData&& other) noexcept;
        void operator=(StructuredData&& other) noexcept;

        StructuredData copy() const;

        const byte* getData() const;
        int getDataSize() const;
        int getElementCount() const;
        const ElemLayout& getLayout() const;

        template <typename T>
        void setValue(int elemIndex, const String& entry, const T& value) {
            setValue(elemIndex, String::Key(entry), value);
        }

        template <typename T>
        void setValue(int elemIndex, const String::Key& entry, const T& value) {
            static_assert(std::disjunction<
                std::is_same<T, float>,
                std::is_same<T, u32>,
                std::is_same<T, Vector2f>,
                std::is_same<T, Vector3f>,
                std::is_same<T, Vector4f>,
                std::is_same<T, Matrix4x4f>,
                std::is_same<T, Color>
            >::value);
            memcpy(data.get() + getDataIndex(elemIndex, entry, sizeof(T)), &value, sizeof(T));
        }

    private:
        int getDataIndex(int elemIndex, const String::Key& entry, int expectedSize) const;

        ElemLayout layout; //don't change this to a pointer, stop preemptively optimizing!!!!!
        std::unique_ptr<byte[]> data;
        int size;
};

}

#endif // PGE_MEMORYLAYOUT_H_INCLUDED
