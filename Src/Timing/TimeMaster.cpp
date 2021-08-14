#include <PGE/Timing/TimeMaster.h>

using namespace PGE;

String TimeMaster::print() const {
    String ret;

    u64 sum = 0;
    for (const auto& [_, child] : top.children) {
        sum += child.time;
    }
    for (const auto& [name, child] : top.children) {
        child.print(ret, sum, 0, name.str);
    }

    return ret;
}
