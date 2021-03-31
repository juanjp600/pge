#ifndef PGE_RESOURCEMANAGER_H_INCLUDED
#define PGE_RESOURCEMANAGER_H_INCLUDED

namespace PGE {

class ResourceBase;
class ResourceManager {
    private:
        ResourceBase** resources;
        int lastIndex;
        int size;

    public:
        ResourceManager(int sz);
        ~ResourceManager();

        void addResource(ResourceBase* resource);

        void increaseSize(int count);
};

}

#endif // PGE_RESOURCEMANAGER_H_INCLUDED
