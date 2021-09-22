#include <PGE/ResourceManagement/ResourceManager.h>

using namespace PGE;

ResourceManager::~ResourceManager() {
    // TODO: C++20 can't come soon enough.
    for (auto it = resources.rbegin(); it != resources.rend(); it++) {
        delete *it;
    }
}
