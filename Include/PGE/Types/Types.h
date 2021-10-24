#ifndef PGE_TYPES_H_INCLUDED
#define PGE_TYPES_H_INCLUDED

#include <stdint.h>
#include <vector>

#include "Reference.h"

namespace PGE {

using byte = unsigned char;

using i8 = int8_t;
using u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;

// We always want 16 bits, but Windows functions expect wchar_t, so this only acts as to avoid unnecessary casting.
// wchar_t is always 16 bits on Windows.
#ifdef _WIN32
using char16 = wchar_t;
#else
using char16 = char16_t;
#endif

template <typename T>
using ReferenceVector = std::vector<Reference<T>>;

}

#endif // PGE_TYPES_H_INCLUDED
