#include <PGE/Timing/Time.h>

using namespace PGE;

static String percentString(u64 divident, u64 divisor) {
    return String::fromFloat((float)((double)divident / divisor * 100.)) + '%';
}

void Timer::print(String& ret, u64 parentTime, int depth, const String& name) const {
    ret += String("-").multiply(depth);
    ret += name + " : " + percentString(time, parentTime);
    ret += '\n';

    u64 babySum = 0;
    for (const auto& [_, child] : children) {
        babySum += child.time;
    }

    if (!children.empty()) {
        ret += String("-").multiply(depth);
        ret += "> " + percentString(time - babySum, time);
        ret += '\n';

        for (const auto& [name, child] : children) {
            child.print(ret, time, depth + 1, name.str);
        }
    }
}