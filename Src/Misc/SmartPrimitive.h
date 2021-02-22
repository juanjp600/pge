#ifndef PGEINTERNAL_SMARTPRIMITIVE_INCLUDED
#define PGEINTERNAL_SMARTPRIMITIVE_INCLUDED

#include <vector>

// TODO: Find a way to get this to work.
//template <typename T, typename S=T, typename = typename std::enable_if<std::is_base_of<T, S>::value || std::is_same<T,S>::value || true>::type >
//class SmartPrimitive;

// TODO: Dedicated SmartPointer/SmartArray?

class SmartBase {
    public:
        virtual ~SmartBase(){};
};

template <typename T, typename S=T>
class SmartPrimitive : public SmartBase {
    public:
        SmartPrimitive(const T& val) {
            destructor = nullptr;
            value = val;
        }
        SmartPrimitive(const T& val, void(*dtor)(const S&)) {
            value = val;
            destructor = dtor;
        }
        ~SmartPrimitive() {
            if (destructor != nullptr) {
                destructor((const S&)value);
            }
        }

        void set(const T& t) {
            value = t;
        }

        T& get() {
            return value;
        }
        T& get() const {
            return value;
        }

        T* getPointer() {
            return &value;
        }

    protected:
        T value;
        void(*destructor)(const S& t);
};

template <typename T>
class SmartRef {
    public:
    // Get.
    T& operator()() {
        return sp->get();
    }
    const T& operator()() const {
        return sp->get();
    }
    // Set.
    void operator=(const T& t) {
        sp->set(t);
    }
    // Get as pointer.
    T* operator&() {
        return sp->getPointer();
    }
    T operator->() {
        return sp->get();
    }

    SmartRef() {}
    SmartRef(SmartPrimitive<T>* sp) {
        this->sp = sp;
    }

    private:
        SmartPrimitive<T>* sp;
};

template <typename T>
static void deletePointer(T* const& t) {
    delete t;
}

template <typename T>
static void clearPointerArray(std::vector<T*> const& v) {
    for (auto& e : v) {
        delete e;
    }
}

class OpContainer {
    public:
        virtual ~OpContainer() {};

        virtual void exec() = 0;
};

class SmartOrderedDestructor {
    public:
        SmartOrderedDestructor(int size) {
            preop = nullptr;
            postop = nullptr;
            objects = new SmartBase*[size];;
            index = 0;
        }

        ~SmartOrderedDestructor() {
            if (preop != nullptr) {
                preop->exec();
            }
            delete preop;
            for (int i = index - 1; i >= 0; i--) {
                delete objects[i];
            }
            delete[] objects;
            if (postop != nullptr) {
                postop->exec();
            }
            delete postop;
        }

        void setPreop(OpContainer* preop) {
            this->preop = preop;
        }

        void setPostop(OpContainer* postop) {
            this->postop = postop;
        }

        template <typename T, class... Args>
        SmartRef<T> getReference(void(*dtor)(const T&), Args&&... args) {
            objects[index] = new SmartPrimitive(T(args...), dtor);
            return SmartRef<T>((SmartPrimitive<T>*)objects[index++]);
        }

        template <typename T, class... Args>
        SmartRef<T> getReference(void(*dtor)(const T&), const T& t) {
            objects[index] = new SmartPrimitive(t, dtor);
            return SmartRef<T>((SmartPrimitive<T>*)objects[index++]);
        }

        template <typename T, typename S, typename... Args>
        SmartRef<T> getReferenceDifferentDestructor(void(*dtor)(const S&), Args&&... args) {
            objects[index] = new SmartPrimitive(T(args...), dtor);
            return SmartRef<T>((SmartPrimitive<T>*)objects[index++]);
        }

        // TODO: Remove? Unused.
        template <typename T, typename... Args>
        SmartRef<T> getReferencePointer(Args&&... args) {
            objects[index] = new SmartPrimitive(new T(args...), deletePointer);
            return SmartRef<T>((SmartPrimitive<T>*)objects[index++]);
        }
    
        template <typename T>
        SmartRef<std::vector<T*>> getReferenceArray(int size=0) {
            objects[index] = new SmartPrimitive(std::vector<T*>(size), clearPointerArray);
            return SmartRef<std::vector<T*>>((SmartPrimitive<std::vector<T*>>*)objects[index++]);
        }

    private:
        OpContainer* preop;
        OpContainer* postop;
        SmartBase** objects;
        int index;
};

#endif // PGEINTERNAL_SMARTPRIMITIVE_INCLUDED
