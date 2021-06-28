#include <PGE/ResourceManagement/ResourceManager.h>

using namespace PGE;

ResourceManager::~ResourceManager() {
    for (auto it = resources.rbegin(); it != resources.rend(); it++) {
        delete *it;
    }
}
