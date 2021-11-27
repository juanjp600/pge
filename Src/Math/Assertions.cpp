#include <numeric>

// These must be true in order for PGE to function properly and portably.
static_assert(std::numeric_limits<float>::is_iec559);
static_assert(sizeof(float) == 4);
