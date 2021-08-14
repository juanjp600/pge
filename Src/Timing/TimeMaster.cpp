#include <PGE/Timing/TimeMaster.h>

using namespace PGE;

String TimeMaster::print() const {
    String ret;

    u64 sum = 0;
    for (const auto& t : top.children) {
        sum += t.second.time;
    }
    for (const auto& t : top.children) {
        t.second.print(ret, sum, 0, t.first.str);
    }

    return ret;
}
