#ifndef PGE_RESOURCEMANAGER_H_INCLUDED
#define PGE_RESOURCEMANAGER_H_INCLUDED

namespace PGE {

class ResourceBase;
class ResourceManager {
    private:
        ResourceBase** resources;
        int nextIndex;
        int size;

    public:
        ResourceManager(int sz);
        virtual ~ResourceManager();

        void addResource(ResourceBase* resource);

        void increaseSize(int count);
};

}

#endif // PGE_RESOURCEMANAGER_H_INCLUDED
