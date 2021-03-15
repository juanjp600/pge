#ifndef PGE_RESOURCEMANAGER_H_INCLUDED
#define PGE_RESOURCEMANAGER_H_INCLUDED

#include <vector>

namespace PGE {

class ResourceOwnerBase;
class ResourceManager {
    private:
        std::vector<ResourceOwnerBase*> resourceOwners;
    public:
        ResourceManager();
        ~ResourceManager();

        void addResource(ResourceOwnerBase* resourceOwner);
};

}

#endif
