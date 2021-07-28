#ifndef PGE_INTERNAL_ABSTRACTIO_H_DEFINED
#define PGE_INTERNAL_ABSTRACTIO_H_DEFINED

#include <fstream>

#include <PGE/File/FilePath.h>
#include <PGE/Exception/Exception.h>
#include <PGE/ResourceManagement/NoHeap.h>

namespace PGE {

/// Utility to more easily deal with file IO streams.
/// Not intended to be polymorphic.
template <typename T>
class AbstractIO : private NoHeap {
    static_assert(std::is_base_of<std::ios_base, T>::value);

    protected:
        static const inline String BAD_STREAM = "Stream turned bad";
        static const inline String INVALID_FILEPATH = "Tried using an invalid path";
    
        T stream;

        AbstractIO(const PGE::FilePath& file, std::ios::openmode mode = std::ios::binary) {
            PGE_ASSERT(file.isValid(), INVALID_FILEPATH);
            stream.open(file.str().cstr(), mode);
            PGE_ASSERT(stream.is_open(), "Could not open (file: \"" + file.str() + "\")");
        }

        void validate() {
            if (!stream.good()) {
                stream.close();
                throw PGE_CREATE_EX(BAD_STREAM);
            }
        }

    public:
        /// Closes the stream prematurely.
        /// Calling this is *not* necessary, the destructor will clean everything up appropriately.
        /// However, possible failure of the close operation will be swallowed in the destructor, while it will be thrown here.
        /// 
        /// Calling *any* further methods after attempting to close (including attempting to close again)
        /// will have no ill effects, but will cause an exception to be raised every call.
        /// @throws #PGE::Exception if closing was not wholly successful.
        void earlyClose() {
            stream.close();
            PGE_ASSERT(stream.good(), BAD_STREAM);
        }
};

}

#endif // PGE_INTERNAL_ABSTRACTIO_H_DEFINED
