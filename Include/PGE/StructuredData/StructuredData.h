#ifndef PGE_MEMORYLAYOUT_H_INCLUDED
#define PGE_MEMORYLAYOUT_H_INCLUDED

#include <PGE/Types/Types.h>
#include <PGE/String/String.h>
#include <PGE/String/Key.h>
#include <PGE/Math/Vector.h>
#include <PGE/Math/Matrix.h>
#include <PGE/Color/Color.h>

#include <unordered_map>

namespace PGE {

class StructuredData {
    public:
        class ElemLayout {
            public:
                struct Entry {
                    Entry();
                    Entry(const String& nm, int sz);

                    String name;
                    int size;
                };

                struct LocationAndSize {
                    LocationAndSize(int loc, int sz);

                    int location;
                    int size;
                };

                ElemLayout();
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

        const byte* getData() const;
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

        ElemLayout layout;
        std::unique_ptr<byte[]> data;
        size_t size;
};

}

#endif // PGE_MEMORYLAYOUT_H_INCLUDED
