#ifndef PGEINTERNAL_SMARTPRIMITIVE_INCLUDED
#define PGEINTERNAL_SMARTPRIMITIVE_INCLUDED

#include <vector>

// TODO: Find a way to get this to work.
//template <typename T, typename S=T, typename = typename std::enable_if<std::is_base_of<T, S>::value || std::is_same<T,S>::value || true>::type >
//class SmartPrimitive;

class SmartBase {
    public:
        virtual ~SmartBase(){};
};

class SmartOrderedDestructor {
    public:
        SmartOrderedDestructor() {
            preop = nullptr;
            postop = nullptr;
            objects = nullptr;
            index = 0;
        }

        ~SmartOrderedDestructor() {
            if (preop != nullptr) {
                preop();
            }
            for (int i = index-1; i >= 0; i--) {
                delete (SmartBase*)objects[i];
            }
            delete[] objects;
            if (postop != nullptr) {
                postop();
            }
        }

        void setPreop(void(*preop)()) {
            this->preop = preop;
        }

        void setPostop(void(*postop)()) {
            this->postop = postop;
        }

        void setSize(int size) {
            if (objects != nullptr) {
                SmartBase** oldObjects = objects;
                objects = new SmartBase*[size];
                index = (index > size ? size : index);
                std::memcpy(objects, oldObjects, index * sizeof(SmartBase*));
                delete[] oldObjects;
            } else {
                objects = new SmartBase*[size];
            }
        }

        void addPointer(SmartBase* ptr) {
            objects[index] = ptr;
            index++;
        }

    private:
        void(*preop)();
        void(*postop)();
        SmartBase** objects;
        int index;
};

template <typename T>
class SmartPointer : public SmartBase {
    public:
        SmartPointer(T* t) {
            ptr = t;
        }

        ~SmartPointer() {
            delete ptr;
        }

    protected:
        T* ptr;
};

template <typename T, typename S=T>
class SmartPrimitive : public SmartBase {
    public:
        void operator=(const SmartPrimitive& sp) {
            value = sp.value;
            destructor = sp.destructor;
            ((SmartPrimitive&)sp).destructor = nullptr;
        }

        // Get.
        T& operator()() {
            return value;
        }
        const T& operator()() const {
            return value;
        }
        // Set.
        void operator=(const T& t) {
            value = t;
        }
        // Get as pointer.
        T* operator&() {
            return &value;
        }

        T* operator->() const {
            return &value;
        }

        T* operator->() {
            return &value;
        }

        SmartPrimitive() {
            destructor = nullptr;
        }
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
                destructor(value);
            }
        }

    protected:
        T value;
        void(*destructor)(const S& t);
};

template <typename T>
class SmartPrimitiveArray : public SmartBase {
    public:
        // Get.
        std::vector<T>& operator()() {
            return values;
        }
        // Index.
        T& operator[](int i) {
            return values[i];
        }

        void operator=(const SmartPrimitiveArray& spa) {
            values = spa.values;
            destructor = spa.destructor;
            ((SmartPrimitiveArray&)spa).destructor = nullptr;
        }

        SmartPrimitiveArray() {
            destructor = nullptr;
        }
        SmartPrimitiveArray(void(*dtor)(const T&)) {
            destructor = dtor;
        }
        SmartPrimitiveArray(int size, void(*dtor)(const T&)) {
            values.resize(size);
            destructor = dtor;
        }
        ~SmartPrimitiveArray() {
            if (destructor != nullptr) {
                for (T val : values) {
                    destructor(val);
                }
            }
        }

    protected:
        std::vector<T> values;
        void(*destructor)(const T& t);
};

#endif // PGEINTERNAL_SMARTPRIMITIVE_INCLUDED
