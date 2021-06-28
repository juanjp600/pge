#ifndef PGE_STREAMUTIL_H_DEFINED
#define PGE_STREAMUTIL_H_DEFINED

#include <PGE/String/String.h>
#include <PGE/Exception/Exception.h>

#define PGE_ASSERT_OPEN(stream, path) PGE_ASSERT((stream).is_open(), "Could not open (file: \"" + (path).str() + "\")")

namespace PGE {

// Internal file stuff.
namespace StreamUtil {
	const String BAD_STREAM("Stream turned bad");
	const String INVALID_FILEPATH("Tried using an invalid path");

	template <class T>
	void safeOpen(T& stream, const FilePath& file, std::ios::openmode mode = 0) {
		static_assert(std::is_base_of<std::ios_base, T>::value);
		PGE_ASSERT(file.isValid(), INVALID_FILEPATH);
		stream.open(file.cstr(), mode);
		PGE_ASSERT_OPEN(stream, file);
	}

	template <class T>
	void safeClose(T& stream) {
		static_assert(std::is_base_of<std::ios_base, T>::value);
		stream.close();
		PGE_ASSERT(stream.good(), BAD_STREAM);
	}

	template <class T>
	void validate(T& stream) {
		static_assert(std::is_base_of<std::ios_base, T>::value);
		if (!stream.good()) {
			stream.close();
			throw PGE_CREATE_EX(BAD_STREAM);
		}
	}
}

}

#endif // PGE_STREAMUTIL_H_DEFINED
