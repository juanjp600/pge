#ifndef PGEINTERNAL_SMARTPRIMITIVE_INCLUDED
#define PGEINTERNAL_SMARTPRIMITIVE_INCLUDED

#include <vector>

// TODO: Find a way to get this to work.
//template <typename T, typename S=T, typename = typename std::enable_if<std::is_base_of<T, S>::value || std::is_same<T,S>::value || true>::type >
//class SmartPrimitive;

// TODO: Dedicated SmartPointer/SmartArray?

class OpContainer {
    public:
        virtual ~OpContainer() {};

        virtual void exec() = 0;
};

class SmartBase {
    public:
        virtual ~SmartBase(){};
};

template <typename T>
class SmartBaseTwo : public SmartBase {
    public:
        void set(const T& t) {
            value = t;
        }

        T& get() {
            return value;
        }

        T* getPointer() {
            return &value;
        }

    protected:
        T value;
};

template <typename T>
class OpContainerFed : public OpContainer {
    public:
        void set(SmartBaseTwo<T>* val) {
            value = val;
        }

    protected:
        SmartBaseTwo<T>* value;
};


template <typename T, typename S=T>
class SmartPrimitive : public SmartBaseTwo<T> {
    public:
        SmartPrimitive(const T& val) {
            destructor = nullptr;
            this->value = val;
        }
        SmartPrimitive(const T& val, void(*dtor)(const S&)) {
            this->value = val;
            destructor = dtor;
        }
        ~SmartPrimitive() {
            if (destructor != nullptr) {
                destructor((const S&)this->value);
            }
        }

    protected:
        void(*destructor)(const S& t);
};

template <typename T>
class SmartCustom : public SmartBaseTwo<T> {
    public:
        SmartCustom(const T& val, OpContainer* op) {
            this->value = val;
            delOp = op;
        }
        ~SmartCustom() {
            delOp->exec();
            delete delOp;
        }

    protected:
        OpContainer* delOp;
};

template <typename T, typename... Args>
class SmartCustomExtra : public SmartBaseTwo<T>{
    public:
        SmartCustomExtra(const T& val, Args... args, void(*dtor)(const T&, Args...)) {
            this->value = val;
            // TODO: Somehow get rid of duplicate data?
            arguments = std::make_tuple(val, args...);
            destructor = dtor;
        }
        ~SmartCustomExtra() {
            std::get<0>(arguments) = this->value;
            std::apply(destructor, arguments);
        }

    private:
        std::tuple<T, Args...> arguments;
        void(*destructor)(const T&, Args...);
};

template <typename T>
class SmartRef {
    public:
        // Get.
        T& operator()() {
            return sb->get();
        }
        const T& operator()() const {
            return sb->get();
        }
        // Set.
        void operator=(const T& t) {
            sb->set(t);
        }
        // Get as pointer.
        T* operator&() {
            return sb->getPointer();
        }
        T operator->() {
            return sb->get();
        }

        SmartRef() {}
        SmartRef(SmartBaseTwo<T>* sb) {
            this->sb = sb;
        }

    private:
        SmartBaseTwo<T>* sb;
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

class SmartOrderedDestructor {
    public:
        SmartOrderedDestructor(int size) {
            preop = nullptr;
            postop = nullptr;
            objects = new SmartBase*[size];
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

        // TODO: Remove? Unused.
        template <typename T>
        SmartRef<T> getReferenceCustom(const T& t, OpContainer* op) {
            objects[index] = new SmartCustom(t, op);
            OpContainerFed<T>* feedOp = dynamic_cast<OpContainerFed<T>*>(op);
            if (feedOp != nullptr) {
                feedOp->set((SmartCustom<T>*)objects[index]);
            }
            return SmartRef<T>((SmartCustom<T>*)objects[index++]);
        }

        // TODO: &&?
        // TODO: Work with pointers for args?
        template <typename T, typename... Args>
        SmartRef<T> getReference(const T& t, Args... args, void(*dtor)(const T&, Args...)) {
            objects[index] = new SmartCustomExtra<T, Args...>(t, args..., dtor);
            return SmartRef<T>((SmartCustomExtra<T, Args...>*)objects[index++]);
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
