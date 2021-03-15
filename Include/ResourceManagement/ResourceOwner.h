#ifndef PGE_RESOURCEOWNER_H_INCLUDED
#define PGE_RESOURCEOWNER_H_INCLUDED

namespace PGE {

class ResourceOwnerBase {
    protected:
        bool holdsResource = false;
        virtual void initInternal() = 0;
    public:
        ResourceOwnerBase() { }
        virtual ~ResourceOwnerBase() { }

        void init() {
            initInternal();
            holdsResource = true;
        }

        bool isHoldingResource() const { return holdsResource; }
};

template <class T>
class ResourceOwner : public ResourceOwnerBase {
    protected:
        T resource;
    public:
        T get() const { return resource; }
};

}

#endif