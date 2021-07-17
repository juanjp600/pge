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
                int getElementSize() const;

                bool operator==(const StructuredData::ElemLayout& other) const;
            private:
                std::unordered_map<String::Key, LocationAndSize> entries;
                int elementSize;
        };

        StructuredData(const ElemLayout& ly, int elemCount);

        const byte* getData() const;

        void setValue(int elemIndex, const String& entryName, float f);
        void setValue(int elemIndex, const String& entryName, u32 u);
        void setValue(int elemIndex, const String& entryName, const Vector2f& v2f);
        void setValue(int elemIndex, const String& entryName, const Vector3f& v3f);
        void setValue(int elemIndex, const String& entryName, const Vector4f& v4f);
        void setValue(int elemIndex, const String& entryName, const Matrix4x4f& m);
        void setValue(int elemIndex, const String& entryName, const Color& c);
    private:
        int getDataIndex(int elemIndex, const String& entryName, int expectedSize) const;

        ElemLayout layout;
        std::vector<byte> data;
};

}

#endif // PGE_MEMORYLAYOUT_H_INCLUDED
