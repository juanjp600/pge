#include <PGE/Timing/Timing.h>

#include <PGE/Timing/TimeMaster.h>

using namespace PGE;

Timing::Timing(TimeMaster& tm, const String& name) : name(name), start(Clock::now()) {
    auto it = tm.current->children.find(name);
    if (it == tm.current->children.end()) {
        tm.current = &tm.current->children.emplace(name, Time{ 0, tm.current }).first->second;
    } else {
        tm.current = &it->second;
    }
    current = &tm.current;
}

Timing::~Timing() {
    (*current)->time += (Clock::now() - start).count();
    *current = (*current)->parent;
}
