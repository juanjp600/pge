#ifndef PGE_TIME_H_INCLUDED
#define PGE_TIME_H_INCLUDED

#include <PGE/String/Key.h>

namespace PGE {

struct Timer {
    u64 time;
    Timer* parent;
    std::unordered_map<String::RedundantKey, Timer> children;
    void print(String& ret, u64 parentTime, int depth, const String& name) const;
};

}

#endif // PGE_TIME_H_INCLUDED